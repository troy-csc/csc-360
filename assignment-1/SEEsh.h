/*
 * SEEsh.h
 * header file for SEEsh.c
 *
 */

void seesh_loop(void);
void sig_exit(int);
char **seesh_split(char *cmd);
int seesh_launch(char **args);
int seesh_execute(char **args);
int num_builtins(void);
int cd(char **args);
int pwd(char **args);
int help(char **args);
int seesh_exit(char **args);
int setvar(char **args);
int unsetvar(char **args);