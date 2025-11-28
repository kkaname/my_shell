#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define INPUT_BUFFER_SIZE 100
#define CMD_AVAILABLE 3 //specifies the number of command suported
#define MAX_COMMAND_LENGTH 1024

//specifing the list of commands the are supported/available to the shell
//defining it as static const because the commands and their names are  fixed and available throughout the program
static const char *builtin_cmd[CMD_AVAILABLE] = {"exit", "echo", "type"};

//this is to implement the type command to check the eligiblity of the command

void execute_type(char *input){
    char *token, *ptr = input;
    char *saveptr1; // State for the outer loop
    token = strtok_r(ptr, " ", &saveptr1);

    while(token != NULL){
        bool is_builtin_cmd = false, is_path_cmd = false;
        for(int i = 0; i < CMD_AVAILABLE; i++){
            if(strcmp(token, builtin_cmd[i]) == 0){
                is_builtin_cmd = true;
            }
        }

        if (is_builtin_cmd){
            printf("%s is a shell builtin\n", token);
        }
        else{
            char *path = strdup(getenv("PATH"));
            if(path != NULL){
                char *saveptr2; // State for the inner loop
                char *dir = strtok_r(path, ":", &saveptr2);
                while(dir != NULL){
                    char full_path[MAX_COMMAND_LENGTH];
                    snprintf(full_path, sizeof(full_path), "%s/%s", dir, token);
                    if(access(full_path, X_OK) == 0){
                        printf("%s is %s\n", token, full_path);
                        is_path_cmd = true;
                        break;
                    }
                    dir = strtok_r(NULL, ":", &saveptr2);
                }
            }
            free(path);
        }
        if (is_path_cmd == false && is_builtin_cmd == false){
            printf("%s: not found\n", token);
        }

        token = strtok_r(NULL, " ", &saveptr1);
    }
}

int main(int argc, char *argv[]) {
	// Flush after every printf
	setbuf(stdout, NULL);
    // defining the buffer to store the input commands
	char input[INPUT_BUFFER_SIZE];
    int i;

	printf("$ ");
	while(fgets(input, sizeof(input), stdin) != NULL){
        //if the input is too long, then we need to clear the input buffer before the next user input
        if (strchr(input, '\n') == NULL){
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
        //remove the '\n' character from the end that is included by the fgets() when the user presses enter Enter
		input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit 0") == 0 || strcmp(input, "exit 1") == 0 || strcmp(input, "exit") == 0){
            exit(0);
        }
        
        //inplementing the echo command
        else if (strncmp(input, "echo ", 5) == 0){
            printf("%s\n", input + 5);
        }

        else if(strncmp(input, "type ", 5) == 0){
            char *ptr = input + 5;
            execute_type(ptr);
        }

        else{
  		    printf("%s: command not found\n", input);
        }
		printf("$ ");
	}

  return EXIT_SUCCESS;
}

