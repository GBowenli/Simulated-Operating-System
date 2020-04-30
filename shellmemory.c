#include<stdlib.h>
#include<stdio.h>
#include<string.h>

struct MEM {
    char *var;
    char *value;
};

struct MEM shellMemory[100];

void init() {
    for (int i = 0; i < 100; i++) {
        shellMemory[i].var = (char*)malloc(100 * sizeof(char));
        shellMemory[i].value = (char*)malloc(100 * sizeof(char));
    }
}

void freeMem() {
    for (int i = 0; i < 100; i++) {
        free(shellMemory[i].var);
        free(shellMemory[i].value);
    }
}

void add(char *newVar, char *newValue) {
    int varLength = 0;
    int newVarLength = strlen(newVar);
    int newValueLength = strlen(newValue);

    for (int i = 0; i < 100; i++) {
        varLength = strlen(shellMemory[i].var);

        if (varLength == 0) {
            strcpy(shellMemory[i].var, newVar);
            strcpy(shellMemory[i].value, newValue);
            return;
        } else {
            if (strcmp(shellMemory[i].var, newVar) == 0) {
                strcpy(shellMemory[i].value, newValue);
                return;
            }
        }
    }
}

char* get(char *reqVar) {
    int varLength = 0;

    for (int i = 0; i < 100; i++) {
        varLength = strlen(shellMemory[i].var);

        if (varLength == 0) {
            return NULL;
        } else {
            if (strcmp(shellMemory[i].var, reqVar) == 0) {
                return shellMemory[i].value;
            }
        }
    }
}