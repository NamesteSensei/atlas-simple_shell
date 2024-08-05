/* File: shell.h
 * Auth: Christopher Wright
 */

#ifndef SHELL_H
#define SHELL_H

void display_prompt(void);
char *read_input(void);
int tokenize_input(char *input, char ***tokens);
int handle_builtins(char **tokens, int last_status);
int execute(char **args);
void free_mem(char *input, char **tokens);
int check_EOF(void);

#endif /* SHELL_H */

