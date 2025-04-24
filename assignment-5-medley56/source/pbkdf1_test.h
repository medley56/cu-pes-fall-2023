/*
 * pbkdf1_test.h
 *
 * Test functions for pbkdf1
 * 
 */

#ifndef _PBKDF1_TEST_H_
#define _PBKDF1_TEST_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* 
 * Compares two byte strings; returns true if they match, false
 * otherwise
 * 
 * Parameters:
 *   b1, b2    The strings to be compared
 *   len       The length of the strings
 */
bool cmp_bin(const uint8_t *b1, const uint8_t *b2, size_t len);

/*
 * Convert a hex string into a binary value
 *
 * Parameters:
 *   out         Output binary value
 *   in_str      Input hex string
 *   binary_len  The length of the binary output, in bytes. 
 *                 in_str must be 2x this length
 * 
 * Returns:
 *   The converted string, in out
 */
void hexstr_to_bytes(uint8_t *out, const char *in_str, size_t binary_len);

/*
 * Convert byte array to a hex string.
 *
 * Parameters:
 *     out_str     Output hex string
 *     in          Input byte array
 *     binary_len  Length of the binary input.
 *
 * Returns:
 *     Hex string in out parameter.
 */
void bytes_to_hexstr(char *out_str, const uint8_t *in_str, size_t binary_len);

/*
 * Tests the isha, hmac_isha(), and pbkdf2_hmac_isha() functions for
 * correctness. Each call returns true if all tests pass, false
 * otherwise. Diagnostic information is printed via printf.
 */

bool test_isha();
bool test_pbkdf1();

#endif  // _PBKDF1_TEST_H_
