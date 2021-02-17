/*
 * CSC 360
 * Assignment 1, Part 2
 * Tarush Roy
 * V00883469
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "SEEsh.h"

// Array of environment variables
extern char **environ;

// Global variables for referencing built-in commands
char *builtin_str[] = {"cd", "pwd", "help", "exit", "set", "unset"};
int (*builtin_func[]) (char **) = {&cd, &pwd, &help, &seesh_exit, &setvar, &unsetvar};

int main(int argc, char **argv) {
    // open .SEEshrc
    FILE *seeshrc = fopen(".SEEshrc", "r");
    if(!seeshrc) {
        fprintf(stderr, "SEEsh: error opening rc file\n");
        exit(0);
    }

    //initialize variables
    size_t lsize = 0;
    char *line = NULL;
    char **tokens;

    /*
     * read rc file by line
     * split line into tokens
     * execute command
     * 
     */
    while(getline(&line, &lsize, seeshrc) != -1) {
        printf("%s", line);
        tokens = seesh_split(line);
        seesh_execute(tokens);
	}
	//free memory
	free(line);
	
	//close rc file
	fclose(seeshrc);    
	
    //shell loop
	seesh_loop();
	
	return 0;
}

// Main loop for the command line of the shell
void seesh_loop(void) {
	int status;
    size_t cmdsize = 0;
    char *cmd = NULL;
    char *ccmp="";
    char **tokens;

    do {
        printf("? ");
        getline(&cmd, &cmdsize, stdin); // read line

        // signal handler for ctrl-c
        if(signal(SIGINT, sig_exit) == SIG_ERR) {
            fprintf(stderr, "SEEsh: signal error");
        }
        // handler for ctrl-d
        if(*cmd == *ccmp) {
            printf("\n");
            exit(0);
        }

        tokens = seesh_split(cmd); // split line into command and arguments
        status = seesh_execute(tokens); // execute command
        // free memory
        free(cmd);
        free(tokens);
    }while(status);
}

// split line into tokens function
char **seesh_split(char *cmd) {
    int maxsize = 512, pos=0;
    char *token;
    char **tokens = malloc(sizeof(char*) * maxsize);
    //malloc check
    if(!tokens) {
        fprintf(stderr, "SEEsh: malloc error\n");
        exit(0);
    }

    token = strtok(cmd, " \n\t"); // first token
    while(token != NULL) {
        tokens[pos++] = token;

        if(pos>maxsize) {
            fprintf(stderr, "SEEsh: command exceeded 512 characters\n");
            exit(0);
        }

        token=strtok(NULL, " \n\t"); // following tokens
    }

    tokens[pos]=NULL;
    return tokens;
}

// fork process and then execute file in the child process
int seesh_launch(char **args) {
    int status;
    pid_t pid;

    pid = fork();
    if(pid == 0) {
        //child process
        if(execvp(args[0], args) == -1) {
            fprintf(stderr, "SEEsh: error executing command\n");
        }
    }
    else if(pid < 0) {
        fprintf(stderr, "SEEsh: error forking\n");
    }
    else {
        //parent process
        //wait till child process has finished executing
        waitpid(pid, &status, WUNTRACED);
    }
    return 1;
}

/*
 * execute function
 * check if command is built-in
 * if yes, run it
 * if no, assume command is an executable file
 * launch executable
 *
 */
int seesh_execute(char **args) {
    int status;

    if(args[0] == NULL) {
        return 1;
    }

    for(int i=0; i<num_builtins(); i++) {
        if(strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    status = seesh_launch(args);
    return status;
}

// return number of built-in commands
int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

// change current directory
int cd(char **args) {
    char *dir = args[1];
    if(dir == NULL) {
        dir = getenv("HOME");
    }
    if(chdir(dir) != 0) {
        fprintf(stderr, "SEEsh: error changing directory\n");
    }

    // set present working directory to the changed directory
    if(setenv("PWD", dir, 1) == -1) {
        fprintf(stderr, "SEEsh: error setting pwd\n");
    }

    return 1;
}

// return current working directory
int pwd(char **args) {
    char *wd = getenv("PWD");
    printf("%s\n", wd);
    return 1;
}

// list all built-in commands
int help(char **args) {
    printf("Built-in commands:\n");
    for(int i=0; i<num_builtins(); i++) {
        printf("    %s\n", builtin_str[i]);
    }
    return 1;
}

// exit the shell
int seesh_exit(char **args) {
    return 0;
}

// set environment variable to the value in the command
int setvar(char **args) {
    int i=0;

    // if name is not provided, list all enironment variables
    if(args[1] == NULL) {
        while(environ[i] != NULL) {
            printf("%s\n", environ[i]);
            i++;
        }
        return 1;
    }
    // if value is not provided, set to empty string
    else if(args[2] == NULL) {
        args[2] = "";
    }

    if(setenv(args[1], args[2], 1) == -1) {
        fprintf(stderr, "SEEsh: error setting environment variable\n");
    }

    return 1;
}

// delete environment variable
int unsetvar(char **args) {
    char *name = args[1];
    if(unsetenv(name) == -1) {
        fprintf(stderr, "SEEsh: environment variable does not exist\n");
    }

    return 1;
}

void sig_exit(int a) {
    exit(0);
}
