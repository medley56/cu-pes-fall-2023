/*
 * dma.h
 *
 *  Created on: Dec 1, 2023
 *      Author: gmedley
 */

#ifndef DMA_H_
#define DMA_H_

#include <stdint.h>

/*
 * @brief Initialize the DMA peripheral
 */
void Init_DMA_For_Playback(uint16_t * source, uint32_t count);

/*
 * @brief Initialize the TPM0 timer that triggers the DMA
 */
void Init_TPM0(uint32_t period_us);

/*
 * @brief Start the TPM0 timer
 */
void Start_TPM0(void);

/*
 * @brief Set up source and dest pointers, the byte counter, and set it running.
 */
void Start_DMA_Playback();

#endif /* DMA_H_ */
