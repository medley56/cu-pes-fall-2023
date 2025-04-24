/*
 * adc.c
 *
 *  Created on: Dec 1, 2023
 *      Author: gmedley
 */


#include "log.h"
#include "autocorrelate.h"

#define ADC_INPUT_CHANNEL (23U)
#define ADC_SAMPLING_FREQ (48000U)  // 48kHz
#define ADC_SAMPLING_PERIOD (21U)  // Sample every 21 microseconds (ideally 20.83us) for ~48kHz sampling
#define SAMPLE_BUFFER_MAX_SIZE (1024U)  // 1024 samples in ADC sample buffer

uint16_t adc_buffer_index = 0;
uint16_t adc_sample_buffer[SAMPLE_BUFFER_MAX_SIZE] = {0};


void Init_TPM1(uint32_t period_us)
{
	//turn on clock to TPM
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;

	//set clock source for tpm
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK;

	// disable TPM
	TPM1->SC = 0;

	//load the counter and mod
	TPM1->MOD = TPM_MOD_MOD(period_us*24);

	//set TPM to trigger DMA interrupts and to _not_ prescale the clock for better resolution
	TPM1->SC = (TPM_SC_DMA_MASK | TPM_SC_PS(1));

	// Enable TPM interrupts for measuring the signal with the ADC
	TPM1->SC |= TPM_SC_TOIE_MASK;

	// Configure NVIC
	NVIC_SetPriority(TPM1_IRQn, 3); // 0, 64, 128 or 192
	NVIC_ClearPendingIRQ(TPM1_IRQn);
	NVIC_EnableIRQ(TPM1_IRQn);
}


void Start_TPM1(void)
{
	// Enable counter
	TPM1->SC |= TPM_SC_CMOD(1);
}


void Init_ADC(void) {
	SIM->SCGC6 |= (1UL << SIM_SCGC6_ADC0_SHIFT);
	ADC0->CFG1 = ADC_CFG1_ADLPC_MASK | ADC_CFG1_ADIV(0) | ADC_CFG1_ADLSMP_MASK |
		ADC_CFG1_MODE(3) | ADC_CFG1_ADICLK(0); // 16 bit conversion mode

	ADC0->SC2 = ADC_SC2_REFSEL(0); // Select default voltage reference pins

	/* And now initialize the timer that triggers an ADC sample (interrupt) */
	Init_TPM1(ADC_SAMPLING_PERIOD);
	/* Start the timer (and interrupts) */
	Start_TPM1();
}


uint16_t Measure_Analog_Voltage(void)
{
	/* Measure the voltage on the ADC input pin, which is also the DAC output pin */
	ADC0->SC1[0] = ADC_INPUT_CHANNEL;

	/* Wait until the measurement is done */
	while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
	{
		// wait
	};

	return ADC0->R[0];
}


void TPM1_IRQHandler(void) {
	//clear pending IRQ
	//LOG("Entering TPM1 IRQ Handler");
	NVIC_ClearPendingIRQ(TPM1_IRQn);
	TPM1->SC |= TPM_SC_TOF_MASK;
	if (adc_buffer_index < SAMPLE_BUFFER_MAX_SIZE)
	{
		/* Unless we have filled the ADC sample buffer, measure the current voltage
		 * and store it in the buffer */
		adc_sample_buffer[adc_buffer_index] = Measure_Analog_Voltage();
		adc_buffer_index++;
	} else {
		/* If we've filled the ADC sample buffer, we have our sample and
		 * we're just waiting for systick to trigger and to analyze it */
		NVIC_DisableIRQ(TPM1_IRQn);
	}
}


void Summarize_Waveform(void)
{
	/* Ready to check results */
	int samples_per_period = autocorrelate_detect_period(adc_sample_buffer, SAMPLE_BUFFER_MAX_SIZE, kAC_16bps_unsigned);
	if (samples_per_period < 0)
	{
		LOG("ERROR: No fundamental frequency detected");
		return;
	}

	uint16_t val = 0, min = 65535, max = 0, sum = 0, mean = 0;
	for (int i=0; i<SAMPLE_BUFFER_MAX_SIZE; i++)
	{
		val = adc_sample_buffer[i];
		sum += val;
		if (val < min)
		{
			min = val;
		}
		if (val > max)
		{
			max = val;
		}
	}
	mean = sum / SAMPLE_BUFFER_MAX_SIZE;
	uint16_t calculated_frequency = ADC_SAMPLING_FREQ / samples_per_period;
	LOG("period=%d samples, min=%d, max=%d, avg=%d, frequency=%d Hz",
		samples_per_period, min, max, mean, calculated_frequency);
}


void Reset_ADC_Sampling(void)
{
	adc_buffer_index = 0;
}

