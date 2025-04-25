/*
 * encoder.c
 *
 *  Created on: Dec 10, 2023
 *      Author: gmedley
 */

#include "encoder.h"

#include "MKL25Z4.h"


#define ENCODER_PORT PORTD  // Only PORTA and PORTD support interrupt generation
#define ENCODER_GPIO GPIOD
#define ENCODER_IRQn 31U  // 31 for PORTD
#define ENCODER_SCGC_PORT_MASK SIM_SCGC5_PORTD_MASK
#define ENCODER_IRQC_MODE 11U  // Either edge (1011)
#define ENCODER_A_PIN 6U
#define ENCODER_B_PIN 7U

#define ENCODER_POLL_PERIOD 5U  // ms

volatile int32_t encoder_value = 0;  // Location of the motor encoder


void encoder_interrupt_init()
{
    /* Send clock to relevant PORT */
    SIM->SCGC5 |= ENCODER_SCGC_PORT_MASK;

    /* Set Pin Control Register to:
     * Set MUX mode to GPIO
     * Enable the pull up resistor (PE)
     * Select pull up (PS)
     * Set IRQC mode to rising edge
     */
    /* FORWARD PIN */
    ENCODER_PORT->PCR[ENCODER_A_PIN] = PORT_PCR_MUX(1) |
                                         PORT_PCR_PE_MASK |
                                         PORT_PCR_PS_MASK |
                                         PORT_PCR_IRQC(ENCODER_IRQC_MODE);
    /* REVERSE PIN */
    ENCODER_PORT->PCR[ENCODER_B_PIN] = PORT_PCR_MUX(1) |
                                         PORT_PCR_PE_MASK |
                                         PORT_PCR_PS_MASK |
                                         PORT_PCR_IRQC(ENCODER_IRQC_MODE);

    /* Configure NVIC */
    NVIC_SetPriority(ENCODER_IRQn, 2);  // Set priority
    NVIC_ClearPendingIRQ(ENCODER_IRQn);  // Clear any pending requests
    NVIC_EnableIRQ(ENCODER_IRQn);  // Enable detection of events

    /* Enabled at reset but rerun in case some other routine has disabled IRQ */
    __enable_irq();
}


int32_t encoder_position(void)
{
    return encoder_value;
}


uint8_t debounce_encoder(uint32_t pin)
{
    uint16_t state = 0b0101010101010101U;  // Initial series of values
    uint16_t mask = 0b0000000011111111U;  // Number of times we check the value
    while (1)
    {
        state = ((state << 1) | ((ENCODER_GPIO->PDIR & (1U << pin)) >> pin)) & mask;
        if (state == mask)
        {
            return 1;
        }
        if (state == 0)
        {
            return 0;
        }
    }
}


void PORTD_IRQHandler(void)
{
    /* Following code sourced from:
     * https://hifiduino.wordpress.com/2010/10/20/rotaryencoder-hw-sw-no-debounce/
     */
    int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
    static uint8_t old_AB = 0;
    /**/
    old_AB <<= 2; // remember previous state
    uint8_t new_AB = (debounce_encoder(ENCODER_A_PIN) << 1) | debounce_encoder(ENCODER_B_PIN);
    ENCODER_PORT->ISFR = 0xFFFFFFFFU;
    old_AB |= new_AB;  // add current state
    if (enc_states[(old_AB & 0x0F)] < 0)
    {
        //LOG("decrementing encoder");
    }
    //LOG("change = %i", enc_states[(old_AB & 0x0F)]);
    encoder_value += ( enc_states[( old_AB & 0x0F )]);

    return;
}
