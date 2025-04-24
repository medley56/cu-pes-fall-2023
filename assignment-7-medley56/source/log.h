/*
 * log.h
 *
 *  Created on: Sep 29, 2023
 *      Author: Gavin Medley
 */

#include "fsl_debug_console.h"

#ifndef LOG_H_
#define LOG_H_

/* The following defines a custom logging macro that is conditional on presence of DEBUG */
#ifdef DEBUG
    #define LOG(fmt, ...) PRINTF(fmt "\r\n", ##__VA_ARGS__)
#else
    #define LOG(fmt, ...) (void)0
#endif

#endif /* LOG_H_ */
