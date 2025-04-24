/*
 * tsi.c
 *
 *  Created on: Sep 26, 2023
 *      Author: Gavin Medley
 */
#include <stdbool.h>
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "log.h"
#include "timing.h"
#include "tsi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define TSI_HIGH_THRESHOLD 300U  // Upper bound for "no touch" range
#define TSI_LOW_THRESHOLD 0U  // Lower bound for "no touch" range
#define TSI_CHANNEL 9U  // Channel for the TSI
#define TSI_NSCN 16U  // Scans NSCN+1 times (zero-indexed)
/* Macro for accessing the TSI oscillation count from the TSI->DATA register */
#define TSI_COUNT (uint32_t)((TSI0->DATA & TSI_DATA_TSICNT_MASK) >> TSI_DATA_TSICNT_SHIFT)
#define TSI_IRQ_PRIORITY 2U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
bool tsi_touched = false;

/*******************************************************************************
 * Code
 ******************************************************************************/
void TSI0_IRQHandler()
{
	tsi_touched = true;  // Set the indicator for touch detected
	// LOG("[%u] TOUCH DETECTED: %u", now(), TSI_COUNT);
	TSI0->GENCS |= TSI_GENCS_OUTRGF_MASK;  // Write 1 to clear out of range flag
	TSI0->GENCS |= TSI_GENCS_EOSF_MASK ; 	// Write 1 to clear end of scan flag
}

void tsi_init()
{
	LOG("Initializing TSI...");
	/* Enable clock signal to TSI */
	SIM->SCGC5 |= SIM_SCGC5_TSI_MASK;

	/* Clear TSICH channel selector */
	TSI0->DATA &= ~TSI_DATA_TSICH_MASK;

	/* Set TSI DATA register to read out specified channel */
	TSI0->DATA |= TSI_DATA_TSICH(TSI_CHANNEL);

	/* The following code chunk is adapted from
	 * https://github.com/alexander-g-dean/ESF/blob/master/NXP/Misc/Touch%20Sense/TSI/src/main.c
	 */
	TSI0->GENCS = TSI_GENCS_MODE(0U) | // Operating in non-noise mode
				  TSI_GENCS_REFCHRG(0U) | // Reference oscillator charge and discharge value 500nA
				  TSI_GENCS_DVOLT(0U) | // Oscillator voltage rails set to default
				  TSI_GENCS_EXTCHRG(0U) | // Electrode oscillator charge and discharge value 500nA
				  TSI_GENCS_PS(0U) |  // Frequency clock divided by one
				  TSI_GENCS_NSCN(TSI_NSCN) | // Scanning the electrode times
				  TSI_GENCS_TSIEN_MASK | // Enabling the TSI module
				  TSI_GENCS_TSIIEN_MASK | // Enable the TSI triggered interrupt
				  TSI_GENCS_EOSF_MASK; // Writing one to clear the end of scan flag

	/* Set TSI scan threshold values. This is board dependent and requires calibration */
	TSI0->TSHD = (TSI_HIGH_THRESHOLD << 16U) | (TSI_LOW_THRESHOLD);

	LOG("Enabling TSI interrupt");
	NVIC_SetPriority(TSI_IRQn, TSI_IRQ_PRIORITY);
	NVIC_ClearPendingIRQ(TSI_IRQn);
	NVIC_EnableIRQ(TSI_IRQn);
	LOG("Done.");
}

void tsi_start_scan()
{
	/* Save interrupt mask state and disable interrupts */
	uint32_t pm = __get_PRIMASK();
	__disable_irq();

	/* Check that the scan in progress flag is not set */
	if (!(TSI0->GENCS & TSI_GENCS_SCNIP_MASK))
	{
		/* Clear end of scan flag. This will be left set if the TSI detected a touch that was
		 * NOT outside of the threshold window (thus not triggering an interrupt) */
		TSI0->GENCS |= TSI_GENCS_EOSF_MASK;

		/* Set software trigger to start scanning */
		TSI0->DATA |= TSI_DATA_SWTS(1);
	}

	__set_PRIMASK(pm);
}
