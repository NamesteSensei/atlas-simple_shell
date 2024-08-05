/* File: shell.c
 * Auth: Christopher Wright
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

/* Function declarations */
void prompt(void);
char *read_line(void);
char **split_line(char *line);
int execute(char **args);
void shell_loop(void);

/**
 * main - Entry point of the shell
 *
 * Return: Always 0.
 */
int main(void)
{
	shell_loop();
	return (0);
}

/**
 * prompt - Displays the prompt
 */
void prompt(void)
{
	printf("($) ");
}

/**
 * read_line - Reads a line of input from stdin
 *
 * Return: The line read from stdin
 */
char *read_line(void)
{
	char *line = NULL;
	size_t bufsize = 0;

	if (getline(&line, &bufsize, stdin) == -1)
	{
		if (feof(stdin))
			exit(EXIT_SUCCESS);
		else
		{
			perror("readline");
			exit(EXIT_FAILURE);
		}
	}

	return (line);
}

/**
 * split_line - Splits a line into tokens
 * @line: The line to split
 *
 * Return: An array of tokens
 */
char **split_line(char *line)
{
	int bufsize = BUFFER_SIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char *));
	char *token;

	if (!tokens)
	{
		fprintf(stderr, "allocation error\n");
		exit(EXIT_FAILURE);
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
				exit(EXIT_FAILURE);
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
		if (execvp(args[0], args) == -1)
		{
			perror("hsh");
		}
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		perror("hsh");
	}
	else
	{
		do {
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return (1);
}

/**
 * shell_loop - The main loop of the shell
 */
void shell_loop(void)
{
	char *line;
	char **args;
	int status;

	do {
		prompt();
		line = read_line();
		args = split_line(line);
		status = execute(args);

		free(line);
		free(args);
	} while (status);
}

