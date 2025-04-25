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
 * @file    assignment1_f23.c
 * @brief
 *
 *
 * @author  Gavin Medley
 * @date    2023-09-06
 *
 */
#ifdef DEBUG
#include <stdio.h>
#include <assert.h>
#include "test_assignment1_f23.h"
#endif
#include "assignment1_f23.h"

#define HEXLOOKUP   (char[]){'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}
#define BITSPERBYTE (8)
#define BITSPERUINT32 (32)

/**
 * @brief      main
 *
 * @param[in]  argc  The count of arguments
 * @param      argv  The arguments array
 *
 * @return     always 0
 */
int32_t main(int32_t argc, char **argv)
{
    #ifdef DEBUG
    puts("Running test suite");

    /*
     * Test helper functions
     */
    assert(test_pow2() == 0);
    assert(test_extract_nth_byte() == 0);
    assert(test_char_is_equal_ignore_case() == 0);

    /*
     * Test Assignment 1 functions 
     */
    assert(test_binsubstr_to_uint() == 0);
    assert(test_asciistr_to_hexstr() == 0);
    assert(test_hamming_distance() == 0);
    assert(test_twiggle_some_bits() == 0);
    assert(test_grab_nbits() == 0);
    assert(test_substring_alt() == 0);

    puts("All tests passed");
    #endif

    return 0;
}

/**
 * @brief      Calculates a power of 2.
 *
 * @param[in]  n  The exponent
 *
 * @return     2^n
 */
uint32_t pow2(int n) 
{
    /* A uint32_t cannot store 2^32 or larger so n must be < 32. */
    if (n > 31 || n < 0) { 
        return ERROR; 
    }
    uint32_t y;
    if (n == 0) {
        y = 1;
    } else {
        y = 2;
        for (int j = 1; j < n; j++) {
            y <<= 1;
        }
    }
    
    return y;
}

/**
 * @brief      Returns an unsigned integer represented by the substring of a string array input(str).
 * 
 * The input string is null terminated. If start_bit + nbits runs beyond the end of the string, an ERROR is returned.
 * Similarly, if any of the characters are not '0' or '1', an ERROR is returned.
 *
 * @param[in]  str        The string
 * @param[in]  start_bit  The start bit
 * @param[in]  nbits      The nbits
 *
 * @return     uint32_t   The unsigned integer representation of the substring.
 */
uint32_t binsubstr_to_uint(const char *str, int start_bit, size_t nbits)
{
    /* Check that start_bit is greater than zero and less than nbits. */
    if (start_bit < 0 || nbits < 1 || nbits > BITSPERUINT32) {
        return ERROR;
    }
    /* Check that the entire str is made of 0 and 1 chars only */
    const char *str_ptr = str;
    while (str_ptr != NULL && *str_ptr != '\0') {
        if (*str_ptr != '1' && *str_ptr != '0') {
            return ERROR;
        }
        str_ptr++;
    }
    /* Initialize a uint32_t to store the integer value. */
    uint32_t value = 0;
    uint32_t bit_value;
    /* Loop through char array starting from start_bit. */
    str += start_bit;
    unsigned int i = 0;
    while (i < nbits) {
        /* If next char is null, we reached the end before we found nbits. Return ERROR. */
        if (str == NULL || *str == '\0') {
            /* Reached end of string before parsing nbits or str pointer was NULL. */
            return ERROR;
        }
        if (*str == '1') {
            /* Calculate the power of 2 to add. */
            bit_value = pow2(i);
            /* If we get an error from pow2, return it. */
            if ( bit_value == ERROR ) {
                return ERROR;
            }
            value += bit_value;
        }
        ++i;
        ++str;
    }
    
    return value;
}

/**
 * @brief      Converts an ASCII string to hex representation and returns the hex string as a null-terminated string.
 *
 * @param[in]  ascii_str  The ascii string
 * @param      hex_str    The hexadecimal string
 * @param[in]  size       The size
 *
 * @return     Hex representation of ascii_str, null-terminated
 */
char *asciistr_to_hexstr(const char *ascii_str, char *hex_str, size_t size)
{
    const int ASCII_VALID_START = 1;
    const int ASCII_VALID_END = 127;
    /* Check first character of string for validity */
    if (ascii_str == NULL || *ascii_str == '\0') {
        return NULL;
    }
    /* Check length of ascii_str and make sure hex_str buffer size is big enough. */
    uint8_t strlen = 0;
    const char *cursor = ascii_str;
    while (*cursor != '\0') {
        /* While we're counting, check that each character in the ascii_str is, in fact, ASCII. */
        if ((uint8_t)*cursor < ASCII_VALID_START || (uint8_t)*cursor > ASCII_VALID_END) {
            return NULL;
        }
        strlen++;
        cursor++;
    }
    /* Check that buffer size is >= 2*strlen + 1 (for hex + null terminator). */
    if (size < (strlen <<= 1) + 1) {
        return NULL;
    }
    /* Set the presumed last element of hex_str buffer to null character. */
    hex_str[size-1] = '\0';
    char *ptr = hex_str;
    uint8_t ascii_num;
    char lhs, rhs;
    /* Loop through ascii_str */
    while (ascii_str != NULL && *ascii_str != '\0') {
        /* Convert each character to an integer */
        ascii_num = (uint8_t)*ascii_str;
        lhs = HEXLOOKUP[ascii_num >> 4];
        rhs = HEXLOOKUP[(uint8_t)(ascii_num << 4) >> 4];
        *hex_str++ = lhs;
        *hex_str++ = rhs;
        ascii_str++;
    }
    /* Append a null terminator to the final hex string. */
    *hex_str = '\0';
    return ptr;
}

/**
 * @brief      Extract a specific byte from a uint32.
 *
 * @param[in]  num    The number
 * @param[in]  n      The byte number where 0 is the least significant byte and 3 is the most
 *
 * @return     The extracted byte, as an uint32_t
 */
uint32_t extract_nth_byte(uint32_t num, int n) 
{
    /* There are only 4 bytes in a 32 bit uint so return ERROR if n > 3 */
    if (n > 3 || n < 0) {
        return ERROR;
    }
    uint32_t subint = num;
    int nleft = 3 - n;
    int nright = n;

    /* Shift left by nleft (3-n) to drop the most significant bits */
    for (int nbytes = 0; nbytes < nleft; nbytes++) {
        subint <<= BITSPERBYTE;
    }

    /* Shift back right by same number of bits, nleft */
    for (int nbytes = 0; nbytes < nleft; nbytes++) {
        subint >>= BITSPERBYTE;
    }

    /* Shift right by nright (n) to drop the least significant bits */
    for (int nbytes = 0; nbytes < nright; nbytes++) {
        subint >>= BITSPERBYTE;
    }
    return subint;
}

/**
 * @brief      Calculate the hamming distance between two bytes of a single 32 bit unsigned integer.
 * 
 * Byte 0 denotes the least significant byte. Byte 3 denotes the most significant byte.
 *
 * @param[in]  num    The number
 * @param[in]  byte1  The byte 1
 * @param[in]  byte2  The byte 2
 *
 * @return     Hamming distance (total number of bit mismatches between bytes).
 */
uint32_t hamming_distance(uint32_t num, int byte1, int byte2)
{
    /* Check to make sure byte1 and byte2 are valid */
    if (byte1 > 3 || byte2 > 3 || byte1 < 0 || byte2 < 0) {
        return ERROR;
    }
    /* Use bit shifts to extract the specified bytes. */
    uint32_t int1 = extract_nth_byte(num, byte1);
    uint32_t int2 = extract_nth_byte(num, byte2);
    uint32_t xored = int1 ^ int2;
    uint32_t count = 0;
    for (int i = 0; i < BITSPERBYTE; i++) {
        if ((xored & 1) == 1) {
            count++;
        }
        xored >>= 1;
    }
    return count;
}

/**
 * @brief      Changes nbits bits of the num starting at start_bit according to the specified operation.
 *
 * @param[in]  num        The number
 * @param[in]  start_bit  The start bit
 * @param[in]  nbits      The nbits
 * @param[in]  operation  The operation
 *
 * @return     The input number with nbits twiggled according to operation.
 */
uint32_t twiggle_some_bits(uint32_t num, int start_bit, int nbits, operation_t operation)
{
    /* Check for valid input */
    if (start_bit + nbits > BITSPERUINT32 || start_bit < 0 || nbits < 0) {
        return ERROR;
    }
    /* Create in integer that has the relevant bits set */
    uint32_t twiggler = 0;
    for (int i = 0; i < nbits; i++) {
        twiggler |= (1 << (i + start_bit));  /* Set bit i + start_bit in twiggler */
    }
    if (operation == CLEAR) {
        num &= ~(twiggler);  /* Clear the bits from num, which are set in twiggler */
    } else if (operation == SET)
    {
        num |= twiggler;  /* Clear the bits from num, which are set in twiggler */
    } else if (operation == TOGGLE)
    {
        num ^= twiggler;  /* Clear the bits from num, which are set in twiggler */
    } else {
        return ERROR;
    }
    return num;
}

/**
 * @brief      Returns nbits bits from the num, shifted down. 
 * 
 * This function’s output is best shown graphically. If start_bit is 20, nbits is 5 and the 32-bit num is represented 
 * in binary as follows:
 *     ....... XXXXX.... ........ ........
 * Then the output would be the five bits represented by the Xs, also in binary: 
 *     00000000 00000000 00000000 000XXXXX
 * Note that start_bit represents the least-significant of the five bits of interest.
 *
 * @param[in]  num        The number
 * @param[in]  start_bit  The start bit
 * @param[in]  nbits      The nbits
 *
 * @return     An integer containing the nbits from num, shifted all the way down.
 */
uint32_t grab_nbits(uint32_t num, int start_bit, int nbits)
{
    /* Check for valid input */
    if (start_bit + nbits > BITSPERUINT32 || start_bit < 0 || nbits < 1) {
        return ERROR;
    }

    /* Slice out a specific bit range using shifts */
    uint32_t subint = num;
    int nleft = BITSPERUINT32 - (start_bit + nbits);
    int nright = start_bit;

    /* Shift left by nleft to drop the most significant bits we don't want */
    for (int nbits = 0; nbits < nleft; nbits++) {
        subint <<= 1;
    }

    /* Shift back right by same number of bits, nleft */
    for (int nbits = 0; nbits < nleft; nbits++) {
        subint >>= 1;
    }

    /* Shift right by nright to drop the least significant bits we don't want */
    for (int nbits = 0; nbits < nright; nbits++) {
        subint >>= 1;
    }
    return subint;
}

/**
 * @brief      Compare two characters, ignoring case for characters A-Z
 *
 * @param[in]  c1       Character 1
 * @param[in]  c2       Character 2 
 *
 * @return     1 means the characters are considered the same. 0 means different.
 */
int char_is_equal_ignore_case(char c1, char c2) 
{
    const int ASCII_LC_DIFF = 0x20;
    const int ASCII_A = 0x41;
    const int ASCII_Z = 0x5A;
    if (c1 >= ASCII_A && c1 <= ASCII_Z) {
        c1 += ASCII_LC_DIFF;  // Convert to lowercase version of character
    }
    if (c2 >= ASCII_A && c2 <= ASCII_Z) {
        c2 += ASCII_LC_DIFF;  // Convert to lowercase version of character
    }
    return c1 == c2;
}

/**
 * @brief      Check if str2 is a substring of str1, ignoring case and spaces.
 * 
 * Note: The case of [A-Za-z] is ignored. All other ASCII characters are considered unique.
 *
 * @param[in]  str1       The string  
 * @param[in]  str2       The string  
 *
 * @return     { description_of_the_return_value }
 */
int substring_alt(const char *str1, const char *str2)
{
    const char *str2_ptr = str2;  // Used to repeatedly iterate through str2
    const char *str1_sub_ptr = str1;  // Used to iterate through a subset of str1 while comparing to str2
    int substr_start = 0;  // Used to keep track of where str2 starts within str1
    
    /* Make sure that both str1 and str2 have length and that str2 is shorter than str2 */
    int str1len = 0;
    while (str1_sub_ptr != NULL && *str1_sub_ptr != '\0') {
        str1len++;
        str1_sub_ptr++;
    }
    /* Reset ptr to beginning of str1 for later iteration */
    str1_sub_ptr = str1; 

    int str2len = 0;
    while (str2_ptr != NULL && *str2_ptr != '\0') {
        str2len++;
        str2_ptr++;
    }
    /* Reset ptr to beginning of str2 for later iteration */
    str2_ptr = str2; 

    if (str2len > str1len || str1len < 1 || str2len < 1) {
        return ERROR;
    }

    /* Loop through characters in str1, skipping spaces. */
    while (str1 != NULL && *str1 != '\0') {
        /* Check that current character is NOT a Space. */
        if (!(*str1 == 0x20)) {
            str1_sub_ptr = str1;
            /* Iterate through str2, incrementing str1_sub_ptr at the same time */
            while (str2_ptr != NULL) {
                /* Skip any spaces in str1 substring */
                if (*str1_sub_ptr == 0x20) {
                    str1_sub_ptr++;
                    continue;
                }

                /* Skip any spaces in str2. */
                if (*str2_ptr == 0x20) {
                    str2_ptr++;
                    continue;
                }

                /* If the next str2 char is the terminator, we have successfully found a complete substring! */
                if (*str2_ptr == '\0') {
                    /* Return str1 index at which we started comparing str1 and str2. */
                    return substr_start;
                }

                /* If the next str1 char is the terminator, we know that we reached the end of str1 without 
                finding a substring. Return the failure code. */
                if (*str1_sub_ptr == '\0') {
                    return ERROR;
                }

                /* Compare current (non-space) str1_sub_ptr character to the current (non-space) str2_ptr character. */
                if (char_is_equal_ignore_case(*str1_sub_ptr, *str2_ptr)) {
                    str2_ptr++;  // Increment str2
                    str1_sub_ptr++;  // Increment str1 substring
                } else {
                    break;  // Break, indicating that we found a char mismatch before str2 terminated.
                }
            }
        }
        /* If we get here, we failed to match, starting at the current substr_start index. */
        substr_start++;  // Increment start location index to the next location
        str1++;  // Increment str1 ptr to next character
        str2_ptr = str2;  // Reset the str2_ptr to the beginning of str2
    }
    /* If we get here, it means we have fully iterated through str1 and reached its null terminator. */
    return ERROR;
}
