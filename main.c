/* File: main.c
 * Auth: Christopher Wright
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
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
    int last_status = 0; /* Exit status of last command */
    int interactive = isatty(STDIN_FILENO); /* Check if input is from a terminal */

    while (1) /* Infinite loop */
    {
        if (interactive)
            display_prompt(); /* Display the shell prompt */
        input = read_input(); /* Read input from the user */
        if (input == NULL) /* Handle end-of-file (Ctrl+D) */
        {
            if (interactive)
                printf("\n"); /* Print a new line */
            break; /* Exit the loop */
        }
        tokens = tokenize_input(input); /* Tokenize the input */
        if (tokens == NULL || tokens[0] == NULL) /* Handle empty input */
        {
            free(input);
            free(tokens);
            continue; /* Continue to the next command */
        }
        if (strcmp(tokens[0], "exit") == 0) /* Handle built-in exit command */
        {
            free(input);
            free(tokens);
            break; /* Exit the loop */
        }
        last_status = execute(tokens); /* Execute the command */
        if (last_status == -1) /* Handle execution failure */
        {
            fprintf(stderr, "./shell: %s: Command not found\n", tokens[0]);
        }
        free(input);
        free(tokens);
    }
    return 0; /* Exit success */
}

