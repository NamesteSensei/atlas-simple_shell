/* File: shell.c
 * Auth: Christopher Wright
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "shell.h"

#define BUFFER_SIZE 1024

/**
 * display_prompt - Displays the prompt
 */
void display_prompt(void)
{
    printf("#cisfun$ ");
}

/**
 * read_input - Reads a line of input from stdin
 *
 * Return: The line read from stdin
 */
char *read_input(void)
{
    char *line = NULL;
    size_t bufsize = 0;

    if (getline(&line, &bufsize, stdin) == -1)
    {
        free(line);
        return (NULL);
    }

    return (line);
}

/**
 * tokenize_input - Splits a line into tokens
 * @line: The line to split
 *
 * Return: An array of tokens
 */
char **tokenize_input(char *line)
{
    int bufsize = BUFFER_SIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if (!tokens)
    {
        fprintf(stderr, "allocation error\n");
        return (NULL);
    }

    token = strtok(line, " \t\r\n\a");
    while (token != NULL)
    {
        tokens[position++] = token;

        if (position >= bufsize)
        {
            bufsize += BUFFER_SIZE;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens)
            {
                fprintf(stderr, "allocation error\n");
                return (NULL);
            }
        }

        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[position] = NULL;
    return (tokens);
}

/**
 * execute - Executes a command
 * @args: The command and its arguments
 *
 * Return: 1 if the shell should continue running, 0 if it should terminate
 */
int execute(char **args)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        /* Child process */
        if (execve(args[0], args, NULL) == -1)
        {
            perror("./shell");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        /* Error forking */
        perror("fork");
    }
    else
    {
        /* Parent process */
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return (status == 0) ? 0 : -1;
}

/**
 * free_mem - Frees allocated memory
 * @input: The input string
 * @tokens: Array of tokens
 */
void free_mem(char *input, char **tokens)
{
    free(input);
    free(tokens);
}

