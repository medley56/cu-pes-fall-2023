/*
 * dma.c
 *
 *  Created on: Dec 1, 2023
 *      Author: gmedley
 */

#include "dma.h"
#include "board.h"
#include "log.h"

#define ADC_CHCFG_SOURCE (54U)  // See Ch 22 of the Reference Manual


/* These globals store the source pointer and number of bytes to copy during playback.
 * When changing waveforms, these should be changed.
 */
uint16_t * Reload_DMA_Source = 0;
uint32_t Reload_DMA_Byte_Count = 0;


void Init_DMA_For_Playback(uint16_t * source, uint32_t count) {
	// Save reload information
	Reload_DMA_Source = source;
	Reload_DMA_Byte_Count = count*2;

	// Gate clocks to DMA and DMAMUX
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

	// Disable DMA channel to allow configuration
	DMAMUX0->CHCFG[0] = 0;

	// Generate DMA interrupt when done
	// Increment source, transfer words (16 bits)
	// Enable peripheral request
	DMA0->DMA[0].DCR = DMA_DCR_EINT_MASK | DMA_DCR_SINC_MASK |
											DMA_DCR_SSIZE(2) | DMA_DCR_DSIZE(2) |
											DMA_DCR_ERQ_MASK | DMA_DCR_CS_MASK;

	// Configure NVIC for DMA ISR
	NVIC_SetPriority(DMA0_IRQn, 2);
	NVIC_ClearPendingIRQ(DMA0_IRQn);
	NVIC_EnableIRQ(DMA0_IRQn);

	// Enable DMA MUX channel with TPM0 overflow as trigger and enable the trigger
	DMAMUX0->CHCFG[0] = DMAMUX_CHCFG_SOURCE(ADC_CHCFG_SOURCE); // | DMAMUX_CHCFG_TRIG_MASK;
}


void Start_DMA_Playback() {
	// initialize source and destination pointers
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) Reload_DMA_Source);  // Copy from the buffer
	DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) (&(DAC0->DAT[0])));  // Copy to the DAC data register
	// byte count
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(Reload_DMA_Byte_Count);
	// clear done flag
	DMA0->DMA[0].DSR_BCR &= ~DMA_DSR_BCR_DONE_MASK;
	// set enable flag
	DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK;
}


void DMA0_IRQHandler(void) {
	// Clear done flag
	DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;
	// Start the next DMA playback cycle
	Start_DMA_Playback();
}


void Init_TPM0(uint32_t period_us)
{
	//turn on clock to TPM
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

	//set clock source for tpm
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK;

	// disable TPM
	TPM0->SC = 0;

	//load the counter and mod
	TPM0->MOD = TPM_MOD_MOD(period_us*24);

	//set TPM to trigger DMA interrupts and to _not_ prescale the clock for better resolution
	TPM0->SC = (TPM_SC_DMA_MASK | TPM_SC_PS(0));
}


void Start_TPM0(void)
{
	// Enable counter
	TPM0->SC |= TPM_SC_CMOD(1);
}
