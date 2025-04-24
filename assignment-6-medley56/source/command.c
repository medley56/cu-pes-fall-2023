/*
 * command.c
 *
 *  Created on: Nov 9, 2023
 *      Author: gmedley
 */

#include "command.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "MKL25Z4.h"
#include "timing.h"
#include "led.h"
#include "constants.h"

#define CR (13U)  // carriage return ASCII decimal code
#define SPACE (32U)  // space ASCII decimal code
#define COMMA (44U)  // comma ASCII decimal code
#define BS (8U)  // backspace ASCII decimal code
#define DEL (127U)  // delete
#define LOWERCASE_A (97U)  // a
#define LOWERCASE_Z (122U)  // z
#define UPPER_LOWER_OFFSET (32U)  // Offset between uppercase and lowercase ASCII
#define MAX_VARARGS (10U)  // maximum number of varargs to command parser

typedef uint32_t (*command_handler_t)(int, char *argv[]);

typedef struct {
	const char *name;
	command_handler_t handler;
	const char *help_string;
} command_table_t;

static const command_table_t led_command_struct = {
	"LED",
	led_command,
	"Changes LED color.\r\nExample:\r\n\t> LED 0xFF00AA 0x0000FF 0x000000 0xFFFFFF\r\n"
};

static const command_table_t echo_command_struct = {
	"ECHO",
	echo_command,
	"Echo text back to yourself.\r\nExample:\r\n\t> ECHO 'Hello, world!'\r\n\t'HELLO, WORLD!'\r\n"
};

static const command_table_t commands[] = {
	echo_command_struct,
	led_command_struct,
};

static const int num_commands = sizeof(commands) / sizeof(command_table_t);

int await_command(char *cmd)
{
	char *cmd_start = cmd;
	char c;
	printf(PROMPT);  // Display command prompt

	/* Await either a command overflow or a carriage return */
	while (1)
	{
		c = getchar();

		/* If getchar returned ERROR, there was no input in the receive buffer */
		if (c == (char) ERROR)
		{
			continue;  // Keep waiting
		}

		if (c == BS)
		{
			/* Backspace has already been output to user
			 * but we need to overwrite the last char with a space and then
			 * backspace again to the correct location. */
			if (cmd == cmd_start)
			{
				/* We are already at the beginning of the command.
				 * The user has already seen the cursor move back.
				 * Move it back forward by printing a space (the last char of the command prompt).
				 * Don't decrement cmd or we will get a memory access error. */
				printf("%c", SPACE);
			} else
			{
				printf("%c%c", SPACE, BS);
				cmd--;  // Go back one char
			}
			continue;
		} else if (c == DEL)
		{
			/* Delete should behave like backspace but unlike backspace, it hasn't
			 * already been displayed to the user. We need to backspace, overwrite
			 * the last char, and then backspace again. */
			if (cmd != cmd_start)
			{
				/* Only do this if we are not at the beginning of the cmd */
				printf("%c%c%c", BS, SPACE, BS);
				cmd--;  // Go back one char
			}
			continue;
		} else if (c == CR)
		{
			/* Indicates command submitted for processing */
			*(cmd++) = c;  // Set next character of cmd
			*(cmd++) = '\0';  // Append a terminator to the cmd buffer
			/* Every command should end with \r\0 at this point */
			break;
		} else if (cmd - cmd_start >= MAX_CMD_LEN - 1)
		{
			/* Command buffer is going to overflow if we keep going */
			NVIC_DisableIRQ(UART0_IRQn);  // Stop accepting characters
			*cmd = '\0';  // Append a terminator to the cmd buffer
			printf("\r\nCommand overflow (MAX_CMD_LEN=%u)\r\n", MAX_CMD_LEN);
			delay(ONE_SECOND); // delay to force user to read the overflow message
			NVIC_EnableIRQ(UART0_IRQn);
			return ERROR;
		}

		*(cmd++) = c;  // Set next character of cmd, then increment cmd
	}
	printf("\r\n");
	//printf("Raw command: %s\r\n", cmd_start);
	return SUCCESS;
}

void process_command(char *cmd)
{
	char *p;
	char *end;

	/* Find end of command string */
	for (end = cmd; *end != '\0'; end++)
	{
	};

	/* Tokenize input in place */
	int in_token = FALSE;
	char *argv[MAX_VARARGS] = { NULL }; // Array of pointers to the token start points in cmd
	int argc = 0;
	memset(argv, 0, sizeof(argv));  // Set all argv elements to 0
	for (p = cmd; p < end; p++)
	{
		if (in_token)
		{
			/* In token (most common) */
			if (*p >= LOWERCASE_A && *p <= LOWERCASE_Z)
			{
				/* In token and current character is an alpha character */
				*p = *p - UPPER_LOWER_OFFSET;  // Uppercase alpha characters
			} else if (*p == SPACE || *p == COMMA || *p == CR)
			{
				/* In token but current character indicates token end */
				in_token = FALSE;
				*p = '\0';  // Rewrite current char as a null terminator
			}
			/* implicit else: In token but char is not an alpha char or a comma/space so just continue */
		} else
		{
			/* Not in token */
			if (*p != SPACE && *p != COMMA && *p != CR)
			{
				/* Start of new token */
				in_token = TRUE;
				argv[argc] = p;
				argc++;
			}

			if (*p >= LOWERCASE_A && *p <= LOWERCASE_Z)
			{
				/* Current character is an alpha character */
				*p = *p - UPPER_LOWER_OFFSET;  // Uppercase alpha characters
			} else if (*p == CR)
			{
				/* Current character is a carriage return. Replace it with a space */
				*p = SPACE;
			}
			/* implicit else: Not in token but current char is a space or comma so just continue */
		}
	}

	if (argc == 0) // no command
	{
		return;
	}

	// Test code. print the tokens
	//print_command(argc, argv);

	for (int i = 0; i < num_commands; i++)
	{
		if (strcasecmp(argv[0], commands[i].name) == 0)
		{
			if (commands[i].handler(argc, argv) == SUCCESS)
			{
				return;
			}
		}
	}

	/* If we arrive here, the command was not recognized
	 * or executing the command produced an error. */
	printf("Unknown command(");
	cat_command(argc, argv);
	printf(")\r\n");
}

uint32_t led_command(int argc, char *argv[])
{
	/* If no args beyond the command, that's an error */
	if (argc < 2)
	{
		return ERROR;
	}

	/* Check if the user is asking for help */
	if (strcasecmp(argv[1], "HELP") == 0)
	{
		printf("%s", led_command_struct.help_string);
		return SUCCESS;
	}

	int rgb;
	char *endptr;
	for (int i = 1; i < argc; i++)
	{
		rgb = strtol(argv[i], &endptr, 0);
		if (rgb > 16777215)
		{
			return ERROR;
		}
		led_control_hex(rgb);
		delay(ONE_SECOND);
	}
	printf("OK\r\n");
	return SUCCESS;
}

uint32_t echo_command(int argc, char *argv[])
{
	if (strcasecmp(argv[1], "HELP") == 0)
	{
		printf("%s", echo_command_struct.help_string);
		return SUCCESS;
	}
	/* Print the command arguments, skipping the command itself (ECHO) */
	for (int i = 1; i < argc; i++)
	{
		printf("%s ", argv[i]);
	}
	printf("\r\n");
	return SUCCESS;
}

void print_command(int argc, char *argv[])
{
	printf("Parsed command: (argc=%u, argv=[", argc);
	printf("%s", argv[0]);
	for (int i = 1; i < argc; i++)
	{
		printf(", %s", argv[i]);
	}
	printf("])\r\n");
}

void cat_command(int argc, char *argv[])
{
	for (int i = 0; i < argc; i++)
	{
		printf("%s ", argv[i]);
	}
	printf("\b");
}

