/* File: main.c
 * Auth: Christopher Wright
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "shell.h"

/**
 * main - Entry point of the shell
 * 
 * Return: 0 on success
 */
int main(void)
{
	char *input = NULL; /* Pointer to input string */
	char **tokens = NULL; /* Array of split input tokens */
	int token_count; /* Number of tokens in input */
	int last_status = 0; /* Exit status of last command */

	while (1) /* Infinite loop */
	{
		display_prompt(); /* Display the shell prompt */
		input = read_input(); /* Read input from the user */
		if (input == NULL) /* Handle end-of-file (Ctrl+D) */
		{
			if (check_EOF() == 1) /* Check for EOF */
			{
				printf("Exiting shell...\n");
				exit(EXIT_SUCCESS); /* Exit gracefully */
			}
			continue; /* Continue to the next command */
		}
		token_count = tokenize_input(input, &tokens); /* Tokenize the input */
		if (token_count == -1) /* Handle tokenization failure */
		{
			free_mem(input, tokens); /* Free allocated memory */
			continue; /* Continue to the next command */
		}
		if (handle_builtins(tokens, last_status) == 0) /* Handle built-in commands */
		{
			last_status = execute(tokens); /* Execute the command */
			if (last_status == -1) /* Handle execution failure */
			{
				free_mem(input, tokens); /* Free allocated memory */
				continue; /* Continue to the next command */
			}
		}
		free_mem(input, tokens); /* Free allocated memory */
		printf("Command executed.\n");
	}
	return (0); /* Exit success */
}

