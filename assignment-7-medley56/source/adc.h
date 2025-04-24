/*
 * adc.h
 *
 *  Created on: Dec 1, 2023
 *      Author: gmedley
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

//extern uint16_t adc_sample_buffer[];
//extern uint16_t adc_buffer_index;

/*
 * @brief Configure the ADC
 */
void Init_ADC(void);

/*
 * Print a summary of the current ADC sample buffer
 */
void Summarize_Waveform(void);

/*
 * @brief Reset the buffer index to zero so we can begin sampling again.
 */
void Reset_ADC_Sampling(void);

#endif /* ADC_H_ */
