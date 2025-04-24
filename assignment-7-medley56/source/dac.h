/*
 * dac.h
 *
 *  Created on: Dec 1, 2023
 *      Author: gmedley
 */

#ifndef DAC_H_
#define DAC_H_

#include <stdint.h>

#define SAMPLE_BUFFER_MAX_SIZE (1024U)  // 1024 samples in buffer
/* Modes for waveform specification */
#define SQUARE_MODE (0U)
#define SINE_MODE (1U)
#define TRIANGLE_MODE (2U)

extern uint16_t sample_buffer[SAMPLE_BUFFER_MAX_SIZE];  // Buffer to store waveform samples for DAC output
extern size_t num_samples;  // Number of valid samples stored in sample buffer


/*
 * @brief Initialize DAC peripheral
 */
void init_dac(void);

/*
 * @brief Rudimentary triangle wave not at any specific frequency.
 *
 * It simply increments up to 4096 and back down to zero
 */
void triangle_output(void);

/*
 * @brief Given a mode, fill the global buffer with up to 1024 waveform sample values.
 *
 * Also set the number of samples global variable because it will vary based on waveform frequency
 * and sampling frequency.
 */
void waveform_to_samples(int mode);

/*
 * @brief Print the sample buffer to the debug console using PRINTF
 */
void print_sample_buffer(void);

/*
 * @brief Prototype function to test playing back a predefined buffer through the DAC
 *
 * Does not use DMA or precise timing. This is just to check the shape of the waveform and look for tearing.
 */
void playback_buffer(void);

#endif /* DAC_H_ */
