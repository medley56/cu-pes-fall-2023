/*******************************************************************************
 * Copyright (C) 2023 by Gavin Medley
 *
 * Redistribution, modification or use of this software in source or binary
 * forms is permitted as long as the files maintain this copyright. Users are
 * permitted to modify this and use it to learn about the field of embedded
 * software. Gavin Medley and the University of Colorado are not liable for
 * any misuse of this material.
 * ****************************************************************************/

/**
 * @file    main.c
 * @brief
 *
 *
 * @author  Gavin Medley
 * @date    2023-09-20
 *
 */

#include <assert.h>
#include "test_llfifo.h"
#include "test_cbfifo.h"

int main(int argc, char **argv)
{
	assert(test_llfifo() == 0);
	assert(test_cbfifo() == 0);
	return 0;
}
