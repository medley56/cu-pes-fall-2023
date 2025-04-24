/*
 * switch.h
 *
 *  Created on: Oct 15, 2023
 *      Author: Gavin Medley
 */

#ifndef SWITCH_H_
#define SWITCH_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SWITCH_IRQn PORTD_IRQn  // 31 for PORTD

extern bool switch_changed;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void switch_init();

#endif /* SWITCH_H_ */
