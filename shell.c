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

extern char **environ;

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
        return NULL;
    }

    return line;
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
        return NULL;
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
                return NULL;
            }
        }

        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[position] = NULL;
    return tokens;
}

/**
 * find_command - Tries to find the command in directories listed in PATH
 * @command: The command to find
 *
 * Return: The full path of the command if found, else NULL
 */
char *find_command(char *command)
{
    char *path_env = NULL;
    char *path, *token, *full_path;
    int i = 0;

    while (environ[i])
    {
        if (strncmp(environ[i], "PATH=", 5) == 0)
        {
            path_env = environ[i] + 5;
            break;
        }
        i++;
    }

    if (path_env == NULL)
    {
        return NULL;
    }

    path = strdup(path_env);
    if (!path)
    {
        fprintf(stderr, "allocation error\n");
        return NULL;
    }

    full_path = malloc(BUFFER_SIZE);
    if (!full_path)
    {
        free(path);
        fprintf(stderr, "allocation error\n");
        return NULL;
    }

    token = strtok(path, ":");
    while (token != NULL)
    {
        snprintf(full_path, BUFFER_SIZE, "%s/%s", token, command);
        if (access(full_path, X_OK) == 0)
        {
            free(path);
            return full_path;
        }
        token = strtok(NULL, ":");
    }

    free(path);
    free(full_path);
    return NULL;
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
    char *cmd_path;

    if (args[0] == NULL)
        return (1); /* An empty command was entered */

    if (access(args[0], X_OK) == 0) {
        cmd_path = args[0]; /* Command is an absolute path */
    } else {
        cmd_path = find_command(args[0]); /* Search for the command in PATH */
    }

    if (cmd_path == NULL)
    {
        fprintf(stderr, "%s: command not found\n", args[0]);
        return (-1);
    }

    pid = fork();
    if (pid == 0)
    {
        /* Child process */
        if (execve(cmd_path, args, NULL) == -1)
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

    if (cmd_path != args[0]) {
        free(cmd_path); /* Only free if it was dynamically allocated */
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

