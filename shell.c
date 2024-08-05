/* File: shell.c
 * Auth: Christopher Wright
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
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
		if (errno == EINTR || errno == EIO)
		{
			free(line);
			return (NULL);
		}
		else
		{
			perror("read_input");
			exit(EXIT_FAILURE);
		}
	}

	return (line);
}

/**
 * tokenize_input - Splits a line into tokens
 * @line: The line to split
 * @tokens: Array of split tokens
 *
 * Return: Number of tokens
 */
int tokenize_input(char *line, char ***tokens)
{
	int bufsize = BUFFER_SIZE;
	int position = 0;
	char *token;
	char *end; /* Moved declaration to the beginning of the function */

	*tokens = malloc(bufsize * sizeof(char *));
	if (!(*tokens))
	{
		fprintf(stderr, "allocation error\n");
		return (-1);
	}

	/* Strip trailing whitespace */
	end = line + strlen(line) - 1;
	while (end > line && (*end == ' ' || *end == '\t' || *end == '\n'))
		*end-- = '\0';

	/* Tokenize the input line */
	token = strtok(line, " \t\r\n\a");
	while (token != NULL)
	{
		(*tokens)[position++] = token;

		if (position >= bufsize)
		{
			bufsize += BUFFER_SIZE;
			*tokens = realloc(*tokens, bufsize * sizeof(char *));
			if (!(*tokens))
			{
				fprintf(stderr, "allocation error\n");
				return (-1);
			}
		}

		token = strtok(NULL, " \t\r\n\a");
	}
	(*tokens)[position] = NULL;
	return (position);
}

/**
 * handle_builtins - Handles built-in commands
 * @tokens: The command and its arguments
 * @last_status: Exit status of last command
 *
 * Return: 0 if the command is a built-in, 1 otherwise
 */
int handle_builtins(char **tokens, int last_status)
{
	(void)last_status;

	if (tokens[0] == NULL)
		return (1);

	if (strcmp(tokens[0], "exit") == 0)
		exit(EXIT_SUCCESS);

	if (strcmp(tokens[0], "cd") == 0)
	{
		if (tokens[1] == NULL)
			fprintf(stderr, "cd: expected argument to \"cd\"\n");
		else
		{
			if (chdir(tokens[1]) != 0)
				perror("cd");
		}
		return (0);
	}

	return (1);
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

	if (args[0] == NULL)
		return (1); /* An empty command was entered */

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

	return (1);
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

