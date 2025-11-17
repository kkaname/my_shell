#include <stdio.h>
#include <stdlib.h>
#include<string.h>

int main(int argc, char *argv[]) {
	// Flush after every printf
	setbuf(stdout, NULL);
	char input[100];
    int i;

	printf("$ ");
	while(fgets(input, sizeof(input), stdin) != NULL){

        //remove the '\n' character from the end that is included by the fgets() when the user presses enter Enter
		input[strcspn(input, "\n")] = '\0';

        if(strcmp(input, "exit 0") == 0 || strcmp(input, "exit 1") == 0){
            exit(0);
        }
        
        else if (strncmp(input, "echo ", 5) == 0){
            char *ptr = input;
            for(i = 5; i < strlen(input); i++){
                printf("%c", ptr[i]);
            }
            printf("\n");
        }

        else if(input){
  		    printf("%s: command not found\n", input);
        }
		printf("$ ");
	}

  return EXIT_SUCCESS;
}
