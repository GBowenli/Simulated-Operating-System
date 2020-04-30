#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "shellmemory.h"
#include "kernel.h"
#include "ram.h"
#include "cpu.h"
#include "memorymanager.h"

char *usedFiles[100];

void initUsedFiles() {
    for (int i = 0; i < 100; i++) {
        usedFiles[i] = (char*)malloc(100*sizeof(char));
    }    
}

void freeUsedFiles() {
    for (int i = 0; i < 100; i++) {
        free(usedFiles[i]);
    }
}

void printError(int errCode) {
    if (errCode == 1) {
        printf("Unknown command\n");
    } else if (errCode == 2) {
        printf("Bye!\n");
    } else if (errCode == 3) {
        printf("Invalid set command\n");
    } else if (errCode == 4) {
        printf("Invalid print command\n");
    } else if (errCode == 5) {
        printf("Invalid run command\n");
    } else if (errCode == 6) {
        printf("Script not found\n");
    } else if (errCode == 7) {
        printf("Script already ran\n");
    } else if (errCode == 8) {
        printf("Variable does not exist\n");
    } else if (errCode == 9) {
        printf("Invalid exec command\n");
    } else if (errCode == 10) {
        printf("Ram overflow error\n");
    }
}

int help() {
    printf("help ------------------------- Displays all the commands\n");
    printf("quit ------------------------- Exits / terminates the shell with \"Bye!\"\n");
    printf("set VAR STRING --------------- Assigns a value to shell memory\n");
    printf("print VAR -------------------- Displays the STRING assigned to VAR\n");
    printf("run SCRIPT.TXT --------------- Executes the file SCRIPT.TXT\n");
    printf("exec 1.TXT 2.TXT 3.TXT ------- Executes all files\n");

    return 0;
}

int set(char *words[4]) {
    if (words[1] == NULL || words[2] == NULL || words[3] != NULL) {
        if (words[3] != NULL) {
            printf("Invalid set command value %s %s cannot contain a space\n", words[2], words[3]);
            return 0;
        }
        return 3;
    } else {
        add(words[1], words[2]);
        return 0;
    }
}

int print(char *words[4]) {
    if (words[2] != NULL) {
        return 4;
    } else {
        if (get(words[1]) != NULL) {
            printf("%s\n", get(words[1]));
            return 0;
        }
        return 8;
    }
}

int runTXT(char *words[4]) {
    if (words[1] == NULL) {
        return 5;
    } else {
        FILE *file = fopen(words[1], "r");
        char line[100];
        int errCode = 0;

        if (file) {
            for (int i = 0; i < 100; i++) {
                if (strlen(usedFiles[i]) == 0) {
                    strcpy(usedFiles[i], words[1]);
                    break;
                } else {
                    if (strcmp(usedFiles[i], words[1]) == 0) {
                        fclose(file);
                        return 7;
                    }
                }
            }   

            while (fgets(line, sizeof(line), file) && errCode == 0) {
                if (line[strlen(line)-2] == '\r') {
                    line[strlen(line)-2] = '\0';
                } 

                words[0] = strtok(line, " ");        
                words[1] = strtok(NULL, " ");
                words[2] = strtok(NULL, " ");
                words[3] = strtok(NULL, "");
                
                errCode = interpret(words); 
                
                printError(errCode);            

                if (errCode != 2) {
                    errCode = 0;
                }
            }
            fclose(file);
            if (errCode == 2) {
                return 100; /* avoid duplicate outputting in shell.c because already outputted above */
            }
            return 0;
        } else {
            return 6;
        }
    }
}

int exec(char *words[4]) {
    if (words[1] == NULL) {
        return 9;
    } 

    int fileOneExist = 1;
    int fileTwoExist= 1;
    int fileThreeExist = 1;

    FILE* fileOne = fopen(words[1], "r");
    fileOneExist = launcher(fileOne);

    if (words[2] != NULL) {
        FILE* fileTwo = fopen(words[2], "r");
        fileTwoExist = launcher(fileTwo);
    }
    if (words[3] != NULL) {
        FILE* fileThree = fopen(words[3], "r");
        fileThreeExist = launcher(fileThree);
    }

    if (fileOneExist == 0 || fileTwoExist == 0 || fileThreeExist == 0) {
        printError(6);
    }
    if (ramOverflow == 1) {
        return 10;
    }

    scheduler();

    // reset everything
    clearRam();
    resetReadyQueue();

    return 0;
}

int interpret(char *words[4]) {
    int errCode = 0;

    if (strcmp(words[0], "help") == 0) {
        errCode = help();           
    } else if (strcmp(words[0], "quit") == 0) {
        errCode = 2;
    } else if (strcmp(words[0], "set") == 0) {
        errCode = set(words);
    } else if (strcmp(words[0], "print") == 0) {
        errCode = print(words);
    } else if (strcmp(words[0], "run") == 0) {
        errCode = runTXT(words);
    } else if (strcmp(words[0], "exec") == 0) {
        errCode = exec(words);
    } else {
        errCode = 1;
    }

    return errCode;
}