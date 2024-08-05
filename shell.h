/* File: shell.h
 * Auth: Christopher Wright
 */

#ifndef SHELL_H
#define SHELL_H

void display_prompt(void);
char *read_input(void);
char **tokenize_input(char *input);
int execute(char **args);
void free_mem(char *input, char **tokens);

#endif /* SHELL_H */

