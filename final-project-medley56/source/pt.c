/*
 * pt.c
 *
 *  Created on: Dec 10, 2023
 *      Author: gmedley
 */


#include "pt.h"

#include <stdio.h>
#include "MKL25Z4.h"

#define PT_PIN (1U)
#define PT_CHANNEL (0U)  // PTE0, ADC0_DP0/ADC0_SE0
#define ADC_OFFSET (60000U)  // Gets subtracted from the ADC value before outputting conversion

void pt_init(void)
{
    /* Send clock to ADC on PORT E */
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

    /* Configure ADC */
    ADC0->CFG1 = ADC_CFG1_ADLSMP_MASK | ADC_CFG1_MODE(3) | ADC_CFG1_ADIV(0) | ADC_CFG1_ADLPC_MASK;

    ADC0->SC2 = ADC_SC2_REFSEL(0);  // Default voltage reference pins

    ADC0->OFS = ADC_OFS_OFS(60000);

    /* NOTE: This further relies on TPM1 being initialized and started */
}


uint16_t pt_read()
{
    volatile uint16_t result = 0;

    ADC0->SC1[0] = PT_CHANNEL | ADC_SC1_DIFF_MASK; // Writing to SC1A triggers a conversion

    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
    {
        // wait
    }

    result = ADC0->R[0];
    return result;
}
