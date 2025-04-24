/*
 * dac.c
 *
 *  Created on: Dec 1, 2023
 *      Author: gmedley
 */

#include "dac.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fp_trig.h"


#define DAC_POS (30U)  // Pin number
#define DAC_PORT PORTE  // PORT for DAC output pin
#define DAC_RESOLUTION (4096U)
#define DAC_HIGH (DAC_RESOLUTION-1)  // 4095
#define DAC_MIDH (DAC_RESOLUTION / 2)  // 2048
#define DAC_MIDL (DAC_MIDH - 1)  // 2047, such that DAC_MIDH + DAC_MIDL = DAC_HIGH
#define DAC_LOW (0U);  // 0
#define DAC_SAMPLE_RATE (96000U)  // 96kHz for all waveforms, as specified in assignment
#define ERROR (0xFFFFFFFFU)  // -1 error code

/* Pre-calculated parameters for each waveform mode */
/* square */
#define SQUARE_SAMPLES_PER_PERIOD (240U)
#define SQUARE_NUM_PERIODS (4U)
#define SQUARE_NUM_SAMPLES (SQUARE_SAMPLES_PER_PERIOD * SQUARE_NUM_PERIODS)  // Number of samples for square wave buffer
#define SQUARE_FREQUENCY (400U)  // 400Hz
/* sine */
#define SINE_SAMPLES_PER_PERIOD (160U)
#define SINE_NUM_PERIODS (6U)
#define SINE_NUM_SAMPLES (SINE_SAMPLES_PER_PERIOD * SINE_NUM_PERIODS)  // Number of samples for sine wave buffer
#define SINE_FREQUENCY (600U)  // 600Hz
/* triangle */
#define TRIANGLE_SAMPLES_PER_PERIOD (120U)
#define TRIANGLE_NUM_PERIODS (8U)
#define TRIANGLE_NUM_SAMPLES (TRIANGLE_SAMPLES_PER_PERIOD * TRIANGLE_NUM_PERIODS)  // Number of samples for triangle wave buffer
#define TRIANGLE_FREQUENCY (800U)  // 800Hz


uint16_t sample_buffer[SAMPLE_BUFFER_MAX_SIZE] = {DAC_HIGH};  // Buffer to store waveform samples for DAC output
size_t num_samples = SAMPLE_BUFFER_MAX_SIZE;  // Number of valid samples stored in sample buffer


void init_dac(void)
{
	/* Send clock signal to DAC0 and PORTE */
	SIM->SCGC6 |= SIM_SCGC6_DAC0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	/* Set PORTE to analog mux mode */
	PORTE->PCR[DAC_POS] &= ~PORT_PCR_MUX_MASK;  // Set to zero (analog mux mode)

	/* Disable buffer mode */
	DAC0->C1 = 0;
	DAC0->C2 = 0;

	/* Enable DAC and choose VDDA as reference voltage
	 * VREFH is connected to the DACREF_1 input (DACRFS = 0)
	 * VDDA is connected to the DACREF_2 input (DACRFS = 1)
	 * */
	DAC0->C0 = DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK;
}


void triangle_output(void)
{
	int i=0, change=1;

	while (1)
	{
		DAC0->DAT[0].DATL = DAC_DATL_DATA0(i);
		DAC0->DAT[0].DATH = DAC_DATH_DATA1(i >> 8);

		i += change;
		if (i == 0)
		{
			change = 1;
		} else if (i == DAC_RESOLUTION-1)
		{
			change = -1;
		}
	}
}


void waveform_to_samples(int mode)
{
	uint16_t val;
	if (mode == SQUARE_MODE)
	{
		PRINTF("Filling buffer with a square wave containing "
				"%d periods, %d samples per period, for a total of %d samples.\r\n",
				SQUARE_NUM_PERIODS, SQUARE_SAMPLES_PER_PERIOD, SQUARE_NUM_SAMPLES);
		/* For our square wave, at 400Hz sampled at 96kHz,
		 * we need to fill our buffer with 4 periods where each
		 * period consists of 240 total samples. 120 samples at 0
		 * and 120 samples at 4095.
		 */
		for (int p = 0; p < SQUARE_NUM_PERIODS; p++)
		{
			for (int i = 0; i < SQUARE_SAMPLES_PER_PERIOD / 2; i++)
			{
				sample_buffer[(p*SQUARE_SAMPLES_PER_PERIOD)+i] = DAC_LOW;  // Set low value
				sample_buffer[((p+1)*SQUARE_SAMPLES_PER_PERIOD)-1-i] = DAC_HIGH;  // Set high value
			}
		}
		num_samples = SQUARE_NUM_SAMPLES;
	} else if (mode == SINE_MODE)
	{
		PRINTF("Filling buffer with a sine wave containing "
				"%d periods, %d samples per period, for a total of %d samples.\r\n",
				SINE_NUM_PERIODS, SINE_SAMPLES_PER_PERIOD, SINE_NUM_SAMPLES);
		/* For our sine wave, at 600Hz,
		 * we need to our buffer with 6 periods, where each period
		 * consists of 960 samples.
		 * The first sample of a period maps to 0, half way through maps to pi.
		 */
		int theta;
		for (int p = 0; p < SINE_NUM_PERIODS; p++)
		{
			theta = 0;
			for (int i = 0; i < SINE_SAMPLES_PER_PERIOD / 2; i++)
			{
				theta = (TWO_PI * i) / SINE_SAMPLES_PER_PERIOD;  // TWO_PI includes the TRIG_SCALE_FACTOR
				sample_buffer[(p*SINE_SAMPLES_PER_PERIOD) + i] = DAC_MIDH + ( ( fp_sin(theta) * DAC_MIDL ) / TRIG_SCALE_FACTOR );
			}
			for (int i = SINE_SAMPLES_PER_PERIOD / 2; i < SINE_SAMPLES_PER_PERIOD; i++)
			{
				theta = (TWO_PI * ( i - SINE_SAMPLES_PER_PERIOD / 2) ) / SINE_SAMPLES_PER_PERIOD;  // TWO_PI includes the TRIG_SCALE_FACTOR
				sample_buffer[(p*SINE_SAMPLES_PER_PERIOD) + i] = -(DAC_MIDH + ( ( fp_sin(theta) * DAC_MIDL ) / TRIG_SCALE_FACTOR ));
			}
		}
		num_samples = SINE_NUM_SAMPLES;
	} else if (mode == TRIANGLE_MODE)
	{
		/* For our triangle wave, at 800Hz,
		 * we need to our buffer with 8 periods, where each period
		 * consists of 960 samples.
		 */
		for (int p = 0; p < TRIANGLE_NUM_PERIODS; p++)
		{
			for (int i = 0; i < TRIANGLE_SAMPLES_PER_PERIOD / 2; i++)
			{
				val = (i * DAC_HIGH * 2) / TRIANGLE_SAMPLES_PER_PERIOD;
				sample_buffer[(p*TRIANGLE_SAMPLES_PER_PERIOD)+i] = val;
				sample_buffer[((p+1)*TRIANGLE_SAMPLES_PER_PERIOD)-1-i] = val;
			}
		}
		num_samples = TRIANGLE_NUM_SAMPLES;
	}
}


void print_sample_buffer(void)
{
	PRINTF("[ ");
	for (int i = 0; i < num_samples; i++)
	{
		PRINTF("%d ", sample_buffer[i]);
	}
	PRINTF("]\r\n");
}


void playback_buffer(void)
{
	int i = 0;
	uint16_t val;
	while (i < num_samples)
	{
		val = sample_buffer[i];
		DAC0->DAT[0].DATL = DAC_DATL_DATA0(val);
		DAC0->DAT[0].DATH = DAC_DATH_DATA1(val >> 8);
		i++;
	}
}
