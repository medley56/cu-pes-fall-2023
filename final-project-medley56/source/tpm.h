/*
 * tpm.h
 *
 * TPM1 is used as a sort of general system monitor that reacts to state changes
 * and takes appropriate action.
 *
 *  Created on: Dec 12, 2023
 *      Author: gmedley
 */

#ifndef TPM_H_
#define TPM_H_

#include <stdint.h>
#include <stdbool.h>

/*
 * @brief Safe mode indicator
 */
extern bool safe_mode;

/*
 * @brief Motor set point value (as commanded)
 */
extern int32_t motor_set_point;



/*
 * @brief Initializes TPM1
 */
void tpm1_init(void);


/*
 * @brief Starts TPM1
 */
void tpm1_start(void);


/*
 * @brief Stops TPM1
 */
void tpm1_stop(void);

#endif /* TPM_H_ */
