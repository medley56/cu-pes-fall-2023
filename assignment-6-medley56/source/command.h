/**
 * @file command.h
 * @brief Header file for command processing functions.
 *
 * @date 2023-11-09
 * @author Gavin Medley
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdint.h>

#define MAX_CMD_LEN (64U) // maximum command length, including terminator
#define PROMPT "$$ "  // this must end with a space

/**
 * @brief Blocking wait loop for the end of a command input string.
 *
 * This function performs a blocking wait until the end of a command input string is detected.
 * It returns if the command buffer overflows or if a carriage return is detected.
 *
 * @param cmd Command buffer.
 */
int await_command(char *cmd);

/**
 * @brief Execute the assembled command.
 *
 * Executes the assembled command contained in the command buffer. If the command is not recognized,
 * it returns an ERROR. This function is adapted from example code in Lecture 21 Slides for ECEN5813
 * Fall 2023, by Lalit Pandit.
 *
 * @param cmd Command buffer (null terminated).
 */
void process_command(char *cmd);

/**
 * @brief Iterate through each commanded color for 1s each.
 *
 * This function iterates through a series of color hex strings,
 * commanding the LED accordingly, and then waiting for 1 second before proceeding to the next color.
 *
 * @param argc Number of arguments (number of colors - 1).
 * @param argv Var args, the LED command followed by a series of color hex strings.
 *
 * @return uint32_t This function returns 0 on success, and 1 in case of an error.
 */
uint32_t led_command(int argc, char *argv[]);

/**
 * @brief Echo back the arguments of the ECHO command.
 *
 * This function echoes back the arguments passed to the ECHO command, which consists of a variable
 * number of space or comma-separated tokens.
 *
 * @param argc Argument count for the ECHO command (number of space/comma separated tokens).
 * @param argv Var args to the ECHO command.
 *
 * @return uint32_t This function returns 0 on success, and 1 in case of an error.
 */
uint32_t echo_command(int argc, char *argv[]);

/**
 * @brief Print command tokens, including the arg count.
 *
 * This function prints the command string based on the provided arguments.
 * The format is `Parsed command: (argc=COMMAND, argv=[ARGV0, ARGV1, ARGV2, ...])`
 *
 * @param argc Argument count.
 * @param argv Arguments.
 */
void print_command(int argc, char *argv[]);

/**
 * @brief Print a command string as a concatenated series of tokens.
 *
 * This function prints the command string based on the provided arguments.
 * The format is `COMMAND ARG1 ARG2 ...`.
 *
 * @param argc Argument count.
 * @param argv Arguments.
 */
void cat_command(int argc, char *argv[]);

#endif /* COMMAND_H_ */
