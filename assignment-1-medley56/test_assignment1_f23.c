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
 * @file    test_assignment1_f23.c
 * @brief
 *
 *
 * @author  Gavin Medley
 * @date    2023-09-06
 *
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "assignment1_f23.h"

/**
 * @brief      Test pow2
 *
 * @return     Success (0) or failure (!=0)
 */
int test_pow2()
{
    fputs("Testing pow2...", stdout);
    /* Test valid inputs */
    assert(pow2(0) == 1);
    assert(pow2(1) == 2);
    assert(pow2(2) == 4);
    assert(pow2(31) == 2147483648);
    /* ERROR when we ask for 2^32 returning a uint32_t. */
    assert(pow2(32) == ERROR);
    puts("Passed");
    return 0;
}

/**
 * @brief      Test extract_nth_byte
 *
 * @return     Success (0) or failure (!=0)
 */
int test_extract_nth_byte() 
{
    fputs("Testing extract_nth_byte...", stdout);
    /* Test valid inputs */
    assert(extract_nth_byte(0b00000000000000010000000000000000, 2) == 1);
    assert(extract_nth_byte(16909320, 0) == 8);
    assert(extract_nth_byte(16909320, 1) == 4);
    assert(extract_nth_byte(16909320, 2) == 2);
    assert(extract_nth_byte(16909320, 3) == 1);
    assert(extract_nth_byte(0b10000000010000000010000000010000, 0) == 16);
    assert(extract_nth_byte(0b10000000010000000010000000010000, 1) == 32);
    assert(extract_nth_byte(0b10000000010000000010000000010000, 2) == 64);
    assert(extract_nth_byte(0b10000000010000000010000000010000, 3) == 128);
    /* Test invalid byte numbers */
    assert(extract_nth_byte(1, 4) == ERROR);
    assert(extract_nth_byte(1, -1) == ERROR);
    puts("Passed");
    return 0;
}

/**
 * @brief      Test char_is_equal_ignore_case
 *
 * @return     Success (0) or failure (!=0)
 */
int test_char_is_equal_ignore_case()
{
    fputs("Testing char_is_equal_ignore_case...", stdout);
    /* Test valid inputs */
    assert(char_is_equal_ignore_case('A', 'a') == 1);
    assert(char_is_equal_ignore_case('A', 'Z') == 0);
    assert(char_is_equal_ignore_case(' ', ' ') == 1);
    assert(char_is_equal_ignore_case('8', '8') == 1);
    assert(char_is_equal_ignore_case('1', 'x') == 0);
    assert(char_is_equal_ignore_case((char)8, (char)8) == 1);
    puts("Passed");
    return 0;
}

/**
 * @brief      Test binsubstr_to_uint
 *
 * @return     Success (0) or failure (!=0)
 */
int test_binsubstr_to_uint() 
{
    fputs("Testing binsubstr_to_uint...", stdout);
    /* Valid inputs */
    assert(binsubstr_to_uint("1000000000000001", 0, 16) == 32769);
    assert(binsubstr_to_uint("11111111111111111", 0, 16) == 65535);
    assert(binsubstr_to_uint("110011", 0, 2) == 3);
    assert(binsubstr_to_uint("11011", 1, 2) == 1);
    assert(binsubstr_to_uint("10101", 2, 3) == 5);
    assert(binsubstr_to_uint("00000000000000000000000000000001", 31, 1) == 1);
    /* ERROR for NULL string */
    assert(binsubstr_to_uint(NULL, 0, 1) == ERROR);
    /* ERROR when start_bit is beyond string length - 1 (counting NULL terminator). */
    assert(binsubstr_to_uint("10101010", 8, 1) == ERROR);  
    /* ERROR when start_bit is acceptable but nbits causes overrun. */
    assert(binsubstr_to_uint("10101010", 6, 5) == ERROR);  
    /* ERROR when nbits is 16 and only 15 non-NULL characters in string. */
    assert(binsubstr_to_uint("111111111111111", 0, 16) == ERROR);  
    /* ERROR when attempting to parse the max value of a 32 bit unsigned integer */
    assert(binsubstr_to_uint("11111111111111111111111111111111", 0, 32) == ERROR);  
    /* Test against largest possible valid integer (one less than max due to ERROR value) */
    assert(binsubstr_to_uint("01111111111111111111111111111111", 0, 32) == 4294967294);  /* 2^32 - 2 = 4294967294 */
    /* ERROR when invalid character (2) in input string */
    assert(binsubstr_to_uint("10121010", 0, 5) == ERROR);
    /* ERROR on invalid (<1) nbits value */
    assert(binsubstr_to_uint("01101011", 5, 0) == ERROR);
    /* ERROR on empty string as input */
    assert(binsubstr_to_uint("", 0, 0) == ERROR);
    /* ERROR on invalid input string */
    assert(binsubstr_to_uint("00111100AB1122C1212ED", 0, 4) == ERROR);
    
    /*
     *Test cases for allowing input strings longer than 32 chars
     */
    assert(binsubstr_to_uint("01111011111111111111111111111111111", 5, 1) == 0);
    /* Test where input string is extra long (allowed) as long as we don't overrun it */
    assert(binsubstr_to_uint("0111111111111111111111111111111110", 32, 2) == 1);
    /* ERROR when input string is extra long (allowed) but start_bit + nbits > strlen */
    assert(binsubstr_to_uint("0111111111111111111111111111111110", 31, 4) == ERROR);
    /* ERROR on invalid (>32) nbits value */
    assert(binsubstr_to_uint("0111111111111111111111111111111110111111111111", 0, 33) == ERROR);

    puts("Passed");
    return 0;
}

/**
 * @brief      Test asciistr_to_hexstr
 *
 * @return     Success (0) or failure (!=0)
 */
int test_asciistr_to_hexstr() 
{
    fputs("Testing asciistr_to_hexstr...", stdout);
    char bigbuf[20];  // Reusable buffer for testing
    /* Test valid inputs */
    assert(strcmp(asciistr_to_hexstr("a", bigbuf, 4), "61") == 0);
    assert(strcmp(asciistr_to_hexstr("CaT", bigbuf, 10), "436154") == 0);
    assert(strcmp(asciistr_to_hexstr("\n", bigbuf, 10), "0A") == 0);
    /* ERROR when size is too small */
    assert(asciistr_to_hexstr("a", bigbuf, 0) == NULL);
    /* ERROR for buffer size too small (i.e. forgot about null terminator) */
    assert(asciistr_to_hexstr("abcde", bigbuf, 10) == NULL);
    /* Test case for a buffer that is exactly the right size. */
    assert(strcmp(asciistr_to_hexstr("CaTCa", bigbuf, 11), "4361544361") == 0);
    /* ERROR for non-ASCII character */
    assert(asciistr_to_hexstr("xyÆz", bigbuf, 9) == NULL);
    /* Success for buffer exactly the right size with spaces */
    assert(strcmp(asciistr_to_hexstr("A to Z", bigbuf, 13), "4120746F205A") == 0);
    /* Success for a string that ends early (treated as short string) */
    assert(strcmp(asciistr_to_hexstr("end\0early", bigbuf, 19), "656E64") == 0);
    /* ERROR for zero length input string */
    assert(asciistr_to_hexstr("", bigbuf, 12) == NULL);
    /* ERROR for null terminator as first char */
    assert(asciistr_to_hexstr("\0early", bigbuf, 12) == NULL);
    /* ERROR for NULL pointer input */
    assert(asciistr_to_hexstr(NULL, bigbuf, 10) == NULL);
    puts("Passed");
    return 0;
}

/**
 * @brief      Test hamming_distance
 *
 * @return     Success (0) or failure (!=0)
 */
int test_hamming_distance()
{
    fputs("Testing hamming_distance...", stdout);
    /* Test valid inputs */
    assert(hamming_distance(0x12, 0, 0) == 0);
    assert(hamming_distance(5, 0, 3) == 2);
    assert(hamming_distance(0b00000000000000001111111100000000, 0, 1) == 8);
    assert(hamming_distance(0b00000000000000001111111101010101, 0, 1) == 4);
    assert(hamming_distance(0b11111111000000001111111101010101, 0, 2) == 4);
    /* ERROR for negative byte index */
    assert(hamming_distance(0x12, 0, -5) == ERROR);
    puts("Passed");
    return 0;
}

/**
 * @brief      Test twiggle_some_bits
 *
 * @return     Success (0) or failure (!=0)
 */
int test_twiggle_some_bits() 
{
    fputs("Testing twiggle_some_bits...", stdout);
    assert(twiggle_some_bits(0, 0, 3, SET) == 7);
    assert(twiggle_some_bits(0, 0, 3, TOGGLE) == 7);
    assert(twiggle_some_bits(0, 0, 32, CLEAR) == 0);
    assert(twiggle_some_bits(0, 31, 1, SET) == 0b10000000000000000000000000000000);
    assert(twiggle_some_bits(0xFFFFFFFFU, 8, 24, CLEAR) == 255);
    assert(twiggle_some_bits(0b11111111000000001111111101010101, 0, 8, TOGGLE) == 0b11111111000000001111111110101010);
    assert(twiggle_some_bits(0b11111111000000001111111101010101, 8, 16, SET) == 0b11111111111111111111111101010101);
    assert(twiggle_some_bits(0, 31, 2, SET) == ERROR);  /* Overrun */
    assert(twiggle_some_bits(0, -1, 3, SET) == ERROR);  /* Negative start_bit */
    assert(twiggle_some_bits(0, 0, -1, SET) == ERROR);  /* Negative nbits */
    puts("Passed");
    return 0;
}

/**
 * @brief      Test grab_nbits
 *
 * @return     Success (0) or failure (!=0)
 */
int test_grab_nbits() 
{
    fputs("Testing grab_nbits...", stdout);
    assert(grab_nbits(0b111011, 2, 2) == 0b10);
    assert(grab_nbits(0b111011, 3, 3) == 0b111);
    assert(grab_nbits(0b1, 0, 35) == ERROR);
    assert(grab_nbits(255, 0, 4) == 15);
    assert(grab_nbits(255, 4, 28) == 15);
    assert(grab_nbits(255, 5, 28) == ERROR);
    assert(grab_nbits(255, -1, 28) == ERROR);
    assert(grab_nbits(255, 0, -1) == ERROR);
    assert(grab_nbits(0, 3, 3) == 0b000);
    assert(grab_nbits(-1, 0, 4) == 15);
    assert(grab_nbits(255, 0, 0) == ERROR);
    puts("Passed");
    return 0;
}

/**
 * @brief      Test substring_alt
 *
 * @return     Success (0) or failure (!=0)
 */
int test_substring_alt() 
{
    fputs("Testing substring_alt...", stdout);
    assert(substring_alt("Cat is  ou  t of the bag", "O u T") == 8);
    assert(substring_alt("President's men ", "D o G") == ERROR);
    assert(substring_alt("0B0033", "0 0") == 2);
    assert(substring_alt("  The quick fox  ", " q u i ckfo x  ") == 6);
    assert(substring_alt("12@#$^&%%*", "@#$^&%%*") == 2);  // %% escapes and becomes a literal %
    assert(substring_alt("12@#$^&%%*", "@#$^&%%*x") == ERROR);
    assert(substring_alt("abcd", "abcdefg") == ERROR);
    assert(substring_alt("thesame", "thesame") == 0);
    assert(substring_alt("extended° x x", "ed°xx") == 6);  // This is neat. It works for extended ASCII too.
    assert(substring_alt(NULL, "astring") == ERROR);
    assert(substring_alt("astring", NULL) == ERROR);
    assert(substring_alt("a", "") == ERROR);
    assert(substring_alt("", "") == ERROR);
    assert(substring_alt("", "a") == ERROR);
    puts("Passed");
    return 0;
}