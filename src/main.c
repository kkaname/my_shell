#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

#define INPUT_BUFFER_SIZE 100
#define CMD_AVAILABLE 4 //specifies the number of command suported
#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGUMENT 100

//specifing the list of commands the are supported/available to the shell
//defining it as static const because the commands and their names are  fixed and available throughout the program
static const char *builtin_cmd[CMD_AVAILABLE] = {"exit", "echo", "type", "pwd"};

void execute_cd (char *cmd) {
    char *arg[MAX_ARGUMENT];
    char *dir, *token, *state;
    char *cd_cmd = cmd;
    token = strtok_r(cd_cmd, " ", &state);
    int i = 0;
    while (token != NULL) {
        arg[i++] = token;
        token = strtok_r(NULL, " ", &state);
    }
    arg[i] = NULL;

    if (arg[1] == NULL || strncmp(arg[1], "~", 1) == 0) {
        dir = getenv("HOME");
        if (dir == NULL) {
            printf("cd: Error set to home directory\n");
            return;
        }
    }
    else if (arg[2] != NULL) {
        printf("cd: Too many arguments\n");
        return;
    }
    else dir = arg[1];

    if (chdir(dir) != 0) {
        printf("cd: %s: No such file or directory\n", dir);
        return;
    }
}


//this function returns the path of the executable
char *find_in_path(char *cmd) {
    if (strchr(cmd, '/')) {
        if (access(cmd, X_OK) == 0) return strdup(cmd);
    }

    char *paths = getenv("PATH");
    if (!paths) return NULL;

    char *path = strdup(paths);
    char *state;
    char *p = strtok_r(path, ":", &state);

    while (p != NULL) {
        char full_path[MAX_COMMAND_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", p, cmd);
        if (access(full_path, X_OK) == 0) {
            free(path);
            return strdup(full_path);
        }
        p = strtok_r(NULL, ":", &state);
    }
    free(path);
    return NULL;
}

//this is to implement the type command to check the eligiblity of the command
void execute_type(char *input) {
    char *token, *ptr = input;
    char *saveptr1; // State for the outer loop
    token = strtok_r(ptr, " ", &saveptr1);

    while(token != NULL) {
        bool is_builtin_cmd = false, is_path_cmd = false;
        for(int i = 0; i < CMD_AVAILABLE; i++) {
            if(strcmp(token, builtin_cmd[i]) == 0) {
                is_builtin_cmd = true;
            }
        }

        if (is_builtin_cmd) {
            printf("%s is a shell builtin\n", token);
        }
        else {
            char *path = find_in_path(token);
            if (path != NULL) {
                printf("%s is %s\n", token, path);
                free(path);
                is_path_cmd = true;
            }
        }

        if (is_path_cmd == false && is_builtin_cmd == false) {
            printf("%s: not found\n", token);
        }

        token = strtok_r(NULL, " ", &saveptr1);
    }
}

void execute_external_command(char *input, char **envp) {
    char *state, *arguments[MAX_ARGUMENT]; //saves the state of token
    char *token = strtok_r(input, " ", &state);
    int i = 0;

    while (token != NULL) {
        arguments[i++] = token;
        token = strtok_r(NULL, " ", &state);
    }
    arguments[i] = NULL;  //cause execve requires NULL terminated array

    if (arguments[0] == NULL) {
        return;
    }

    char *path = find_in_path(arguments[0]);
    if (!path) {
        printf("%s: command not found\n", arguments[0]);
        return;
    }

    pid_t pid = fork();

    if (pid == 0) {
        //this is a child process
        execve(path, arguments, envp);
        perror("execve");
        exit(1);
    }
    else if (pid == -1) {
        perror("fork");
        return;
    }
    else {
        // parent process waits
        waitpid(pid, NULL, 0);
    }

    free(path);
}

int main(int argc, char *argv[], char *envp[]) {
	// Flush after every printf
	setbuf(stdout, NULL);
    // defining the buffer to store the input commands
	char input[INPUT_BUFFER_SIZE];

	printf("$ ");
	while(fgets(input, sizeof(input), stdin) != NULL) {
        //if the input is too long, then we need to clear the input buffer before the next user input
        if (strchr(input, '\n') == NULL) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
        //remove the '\n' character from the end that is included by the fgets() when the user presses enter Enter
		input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit 0") == 0 || strcmp(input, "exit 1") == 0 || strcmp(input, "exit") == 0) {
            exit(0);
        }

        //inplementing the echo command
        else if (strncmp(input, "echo ", 5) == 0) {
            printf("%s\n", input + 5);
        }

        else if(strncmp(input, "type ", 5) == 0) {
            char *ptr = input + 5;
            execute_type(ptr);
        }

        else if (strncmp(input, "pwd", 3) == 0) {
            char pwd[1024];
            if (getcwd(pwd, sizeof(pwd)) != 0) {
                printf("%s\n", pwd);
            }
            else {
                perror("getcwd() error");
            }
        }

        else if (strncmp(input, "cd", 2) == 0) {
            execute_cd(input);
        }

        //implement executing commands through PATH and provide them with arguments
        else if (input[0] != '\0') {
            execute_external_command(input, envp);
        }

        else {
  		    printf("%s: command not found\n", input);
        }
		printf("$ ");
	}

  return EXIT_SUCCESS;
}
