/*
 * led.c
 * 
 * Implementation file for LED control functions
 *
 *  Created on: Sep 25, 2023
 *      Author: Gavin Medley
 */

#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "led.h"
#include "log.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Clock gating */
#define LED_SCGC (SIM->SCGC5)
#define LED_SCGC_MASK (SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK)  // Sends clock to Ports B and D
#define TPM_SCGC (SIM->SCGC6)
#define TPM_SCGC_MASK (SIM_SCGC6_TPM0_MASK | SIM_SCGC6_TPM2_MASK)  // Sends clock to TPM 0 and 1
/* LED Port and Pin control */
#define RED_LED_PORT PORTB
#define GREEN_LED_PORT PORTB
#define BLUE_LED_PORT PORTD
#define RED_LED_PIN (18U)  // RED is pin 18 on Port B
#define GREEN_LED_PIN (19U)  // GREEN is pin 19 on Port B
#define BLUE_LED_PIN (1U)  // BLUE is pin 1 on Port D
#define RED_LED_GPIO GPIOB
#define GREEN_LED_GPIO GPIOB
#define BLUE_LED_GPIO GPIOD
/* Multiplexing modes */
#define RED_PWM_MUX_MODE (3U)  // 0011 MUX mode for Port B Pin 18
#define GREEN_PWM_MUX_MODE (3U)  // 0011 MUX mode for Port B Pin 19
#define BLUE_PWM_MUX_MODE (4U)  // 0100 MUX mode for Port D Pin 1
/* PWM configuration
 * RED = TPM2_CH0
 * GREEN = TPM2_CH1
 * BLUE = TPM0_CH1 */
#define RED_TPM TPM2
#define GREEN_TPM TPM2
#define BLUE_TPM TPM0
#define RED_TPM_CH (0U)
#define GREEN_TPM_CH (1U)
#define BLUE_TPM_CH (1U)
#define TPM_SOPT (SIM->SOPT2)
#define TPM_CLK_SOURCE (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK)

#define PWM_PERIOD (255U)  // Allows for PWM duty cycle control between 0/255 and 255/255
#define MASK(x) (1UL << (x))  // Adapted from Alexander Dean book, p. 40

/*******************************************************************************
 * Code
 ******************************************************************************/

void led_init()
{
	LOG("Initializing LED...");
	/* ***********************
	 * v LED Configuration v *
	 *************************/
	/* Enable clock signal to PORTB and PORTD */
	LED_SCGC |= LED_SCGC_MASK;
	/* Clear MUX setting for Pin Control Register (PCR) */
	RED_LED_PORT->PCR[RED_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	/* Set PCR to PWM MUX mode */
	RED_LED_PORT->PCR[RED_LED_PIN] |= PORT_PCR_MUX(RED_PWM_MUX_MODE);
	/* Same for green and blue pins */
	GREEN_LED_PORT->PCR[GREEN_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	GREEN_LED_PORT->PCR[GREEN_LED_PIN] |= PORT_PCR_MUX(GREEN_PWM_MUX_MODE);
	BLUE_LED_PORT->PCR[BLUE_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	BLUE_LED_PORT->PCR[BLUE_LED_PIN] |= PORT_PCR_MUX(BLUE_PWM_MUX_MODE);

	/* ***********************
	 * v TPM Configuration v *
	 *************************/
	/* Enable clock to TPM modules */
	TPM_SCGC |= TPM_SCGC_MASK;

	/* Configure TPM clock source */
	TPM_SOPT |= TPM_CLK_SOURCE;  // Use Phase-Locked Loop clock as TPM clock source

	/* RED */
	RED_TPM->MOD = PWM_PERIOD;  // Consistent period across all LEDs
	RED_TPM->SC = TPM_SC_PS(1);  // Set divide by 2 prescaler
	RED_TPM->CONF |= TPM_CONF_DBGMODE(3);  // Enable PWM counter in debug mode
	RED_TPM->CONTROLS[RED_TPM_CH].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK;  // edge-aligned low-true PWM
	RED_TPM->CONTROLS[RED_TPM_CH].CnV = 0;  // Set initial duty cycle to 0
	RED_TPM->SC |= TPM_SC_CMOD(1);  // Start TPM

	/* GREEN */
	GREEN_TPM->MOD = PWM_PERIOD;  // Consistent period across all LEDs
	GREEN_TPM->SC = TPM_SC_PS(1);  // Set divide by 2 prescaler
	GREEN_TPM->CONF |= TPM_CONF_DBGMODE(3);  // Enable PWM counter in debug mode
	GREEN_TPM->CONTROLS[GREEN_TPM_CH].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK;  // edge-aligned low-true PWM
	GREEN_TPM->CONTROLS[GREEN_TPM_CH].CnV = 0;  // Set initial duty cycle to 0
	GREEN_TPM->SC |= TPM_SC_CMOD(1);  // Start TPM

	/* BLUE */
	BLUE_TPM->MOD = PWM_PERIOD;  // Consistent period across all LEDs
	BLUE_TPM->SC = TPM_SC_PS(1);  // Set divide by 2 prescaler
	BLUE_TPM->CONF |= TPM_CONF_DBGMODE(3);  // Enable PWM counter in debug mode
	BLUE_TPM->CONTROLS[BLUE_TPM_CH].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK;  // edge-aligned low-true PWM
	BLUE_TPM->CONTROLS[BLUE_TPM_CH].CnV = 0;  // Set initial duty cycle to 0
	BLUE_TPM->SC |= TPM_SC_CMOD(1);  // Start TPM

	LOG("Done.");
}

void led_control(uint8_t r, uint8_t g, uint8_t b)
{
	RED_TPM->CONTROLS[RED_TPM_CH].CnV = (uint32_t)r;
	GREEN_TPM->CONTROLS[GREEN_TPM_CH].CnV = (uint32_t)g;
	BLUE_TPM->CONTROLS[BLUE_TPM_CH].CnV = (uint32_t)b;
}

void led_control_hex(uint32_t rgb)
{
	uint8_t r = (uint8_t)((rgb << 8U) >> 24U);
	uint8_t g = (uint8_t)((rgb << 16U) >> 24U);
	uint8_t b = (uint8_t)((rgb << 24U) >> 24U);
	led_control(r, g, b);
}
