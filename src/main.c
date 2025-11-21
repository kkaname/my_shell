#include <stdio.h>
#include <stdlib.h>
#include<string.h>

#define CMD_AVAILABLE 3

static const char *builtin_cmd[CMD_AVAILABLE] = {"exit", "echo", "type"};

int type(char *command){
    for(int i = 0; i < CMD_AVAILABLE; i++){
        if(strcmp(command, builtin_cmd[i]) == 0){
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
	// Flush after every printf
	setbuf(stdout, NULL);
	char input[100];
    int i;

	printf("$ ");
	while(fgets(input, sizeof(input), stdin) != NULL){

        //remove the '\n' character from the end that is included by the fgets() when the user presses enter Enter
		input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit 0") == 0 || strcmp(input, "exit 1") == 0 || strcmp(input, "exit") == 0){
            exit(0);
        }
        
        else if (strncmp(input, "echo ", 5) == 0){
            char *ptr = input;
            for(i = 5; i < strlen(input); i++){
                printf("%c", ptr[i]);
            }
            printf("\n");
        }
        

        else if(strncmp(input, "type ", 5) == 0){
            char *token, *ptr = &input[5];
            int res;
            token = strtok(ptr, " ");
            while(token != NULL){
                res = type(token);
                if(res == 0){
                    printf("%s is a shell builtin\n", token);
                } 
                else{
                    printf("%s: not found\n", token);
                }
                token = strtok(NULL, " ");
            }
        }

        else{
  		    printf("%s: command not found\n", input);
        }
		printf("$ ");
	}

  return EXIT_SUCCESS;
}

