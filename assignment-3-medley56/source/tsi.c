/*
 * tsi.c
 *
 *  Created on: Sep 26, 2023
 *      Author: Gavin Medley
 */

#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "tsi.h"
#include "log.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define ERROR 0xFFFFFFFFU
/* Macro for accessing the TSI oscillation count from the TSI->DATA register */
#define TSI_COUNT (uint32_t)((TSI0->DATA & TSI_DATA_TSICNT_MASK) >> TSI_DATA_TSICNT_SHIFT)
#define ZERO 0U  // Zero, unsigned

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
void tsi_init()
{
	/* Enable clock signal to TSI */
	SIM->SCGC5 |= SIM_SCGC5_TSI_MASK;

	/* The following code chunk is adapted from
	 * https://github.com/alexander-g-dean/ESF/blob/master/NXP/Misc/Touch%20Sense/TSI/src/main.c
	 */
	TSI0->GENCS = TSI_GENCS_MODE(ZERO) | // Operating in non-noise mode
				  TSI_GENCS_REFCHRG(ZERO) | // Reference oscillator charge and discharge value 500nA
				  TSI_GENCS_DVOLT(ZERO) | // Oscillator voltage rails set to default
				  TSI_GENCS_EXTCHRG(ZERO) | // Electrode oscillator charge and discharge value 500nA
				  TSI_GENCS_PS(ZERO) |  // Frequency clock divided by one
				  TSI_GENCS_NSCN(TSI_NSCN) | // Scanning the electrode times
				  TSI_GENCS_TSIEN_MASK | // Enabling the TSI module
				  TSI_GENCS_EOSF_MASK; // Writing one to clear the end of scan flag
	return;
}


uint32_t tsi_read(uint32_t tsich)
{
	/* Clear TSICH channel selector */
	TSI0->DATA &= ~TSI_DATA_TSICH_MASK;

	/* Check for valid channel specification */
	if (tsich == TSI_LHS_CHANNEL || tsich == TSI_RHS_CHANNEL)
	{
		/* Set TSI DATA register to read out specified channel */
		TSI0->DATA |= TSI_DATA_TSICH(tsich);
	} else {
		return ERROR;
	}

	/* Set software trigger to start scanning */
	TSI0->DATA |= TSI_DATA_SWTS(1);

	/* Wait until the end of scan flag is 1 */
	while (!(TSI0->GENCS & TSI_GENCS_EOSF_MASK))
	{
		continue;
	}
	/* Store the value before clearing the EOSF bit */
	uint32_t calibrated_count = TSI_COUNT - TSI_OFFSET;
	TSI0->GENCS |= TSI_GENCS_EOSF_MASK ; // writing one to clear the end of scan flag
	/* Read out the number of counts from TSICNT */
	return calibrated_count;
}
