/*
 * tpm.c
 *
 *  Created on: Dec 12, 2023
 *      Author: gmedley
 */

#include "tpm.h"

#include <stdio.h>
#include "MKL25Z4.h"
#include "pidctl.h"
#include "pt.h"
#include "led.h"

#define PT_THRESHOLD (6000U)  // Threshold for "light detected"
#define TPM_OVERFLOW_MOD (375U)  // 48MHz / 128 = 375kHz. If we count 375 then we should be counting milliseconds
#define TPM_SC_PRESCALE (0b111)  // Prescale by 128
#define PT_MONITOR_MOD (10U)  // Check the photo transistor for light every 10ms
#define MOTOR_CTRL_MOD (100U)  // Check and update the motor control

bool safe_mode = false;


void tpm1_init(void)
{
    printf("tpm1_init\r\n");
    //turn on clock to TPM
    SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;

    // Set TPM clock source as MCGPLLCLK no divide by two (no PLLFLLSEL mask)
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

    // disable TPM
    TPM1->SC = 0;

    // Prescale the TPM by 64 (note that the counter must be disabled, i.e. CMOD set to 0)
    TPM1->SC |= (TPM_SC_PS(0b110));

    // Set the mod for overflow
    TPM1->MOD = TPM_MOD_MOD(TPM_OVERFLOW_MOD);

    // Enable TPM interrupts for measuring the signal with the ADC
    TPM1->SC |= TPM_SC_TOIE_MASK;

    // Configure NVIC
    NVIC_SetPriority(TPM1_IRQn, 3); // 0, 64, 128 or 192
    NVIC_ClearPendingIRQ(TPM1_IRQn);
    NVIC_EnableIRQ(TPM1_IRQn);
}


void tpm1_start(void)
{
    // Enable counter
    TPM1->SC |= TPM_SC_CMOD(1);
}


void tpm1_stop(void)
{
    // Disable counter
    TPM1->SC |= TPM_SC_CMOD(0);
}


/*
 * @brief Take instrument to SAFE mode
 */
void go_safe(void)
{
    safe_mode = true;
    /* Bring motor to safe position */
    set_point = 0;
    tolerance_met = false;
    led_control_hex(0xff0000);
}


/*
 * @brief IRQ Handler for TPM1, which handles system state changes
 */
void TPM1_IRQHandler(void) {

    /* The following counters allow us to use one TPM for multiple purposes at different rates */
    static uint32_t motor_ctrl_counter;  // Only update motor control every so often
    static uint32_t pt_monitor_counter;  // Only check PT every so often
    static uint16_t pt_value;

    NVIC_ClearPendingIRQ(TPM1_IRQn);
    TPM1->SC |= TPM_SC_TOF_MASK;

    if (safe_mode && tolerance_met)
    {
        /* Disable TPM interrupt for light sensing and motor control */
        led_control_hex(0xffff00);  // Yellow indicator for safe and sound
        NVIC_DisableIRQ(TPM1_IRQn);
        printf("In safe mode. Motor and PT update disabled.");
    }


    /* Execute this block only when out of SAFE mode and PT monitor mod has overflowed */
    if (pt_monitor_counter >= PT_MONITOR_MOD && !safe_mode)
    {
        //printf("Checking PT\r\n");
        pt_value = pt_read();
        printf("PT: %d\r\n", pt_value);
        if (pt_value > PT_THRESHOLD)
        {
            /* Go to SAFE mode */
            printf("Going SAFE");
            go_safe();
        }
        pt_monitor_counter = 0;
    }

    if (motor_ctrl_counter >= MOTOR_CTRL_MOD)
    {
        //printf("Updating motor drive\r\n");
        /* Update motor control */
        update_pid();
        motor_ctrl_counter = 0;
    }

    motor_ctrl_counter++;
    pt_monitor_counter++;

}
