#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "shellmemory.h"
#include "interpreter.h"

int shellUI() {
    char input[100];
    char *words[4];
    int errCode = 0;

    initUsedFiles();
    init();
    printf("Welcome to the Bowen shell!\n");
    printf("Version 3.0 Updated March 2020\n");

    while (errCode == 0) {
        printf("$ ");
        fgets(input, 100, stdin);

        int index = 0;
        int oldStrlen;

        while(input[index] == ' ') {
            index++;
        }
        
        oldStrlen = strlen(input);

        if (index != 0) { /* remove the initial white spaces */
            for (int i = 0; i < oldStrlen - index; i++) {
                input[i] = input[i + index];
            }
        }
        input[oldStrlen - index] = '\0';
        
        if (input[strlen(input)-2] == '\r') { /* removes the \r in \r\n if the input is piped in the shell  */
            input[strlen(input)-2] = '\0';
        } else {
            if (input[strlen(input)-1] == '\n') { /* removes \n if the input is user input */
                input[strlen(input)-1] = '\0';
            }
        }    

        for (int i = strlen(input)-1; i >= 0; i--) { // remove all trailing spaces 
            if (input[i] == ' ') {
                input[i] = '\0';
            } else {
                break;
            }
        }

        words[0] = strtok(input, " ");        
        words[1] = strtok(NULL, " ");
        words[2] = strtok(NULL, " ");
        words[3] = strtok(NULL, "");
        
        errCode = interpret(words); 
    }
    printError(errCode);

    freeUsedFiles();
    freeMem();
    return(0);
}