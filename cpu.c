#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include "ram.h"
#include "interpreter.h"
#include "pcb.h"

struct CPU { 
    int IP;
    int offset;
    char IR[1000];
    int quanta;
};

struct CPU cpu = {0, 0, "", 2}; // initialzie cpu

int cpuAvailable() {
    if (cpu.quanta == 0 || strlen(cpu.IR) == 0) {
        return 1;
    } 
    return 0;
}

void setIP (int PC) {
    cpu.IP = PC;
}

void setOffset(int offset) {
    cpu.offset = offset;
}

int run(int quanta) {
    int errCode = 0;
    char *words[4];

    char* completeScript = getRam(cpu.IP);
    int end;

    while (quanta != 0) {
        int totalNewLines = 0;
        int startIndex = 0;

        for (int i = 0; i < 1000; i++) {
            if (totalNewLines == cpu.offset) {
                break;
            } else {
                if (completeScript[i] != '\0') {
                    if (completeScript[i] == '\n') {
                        totalNewLines++;
                        startIndex = i+1;
                    }
                } else {
                    break;
                }
            }
        }

        for (int i = 0; i < 1000; i++) {
            if (completeScript[i] != '\0') {
                if (completeScript[startIndex + i] != '\r') {
                    cpu.IR[i] = completeScript[startIndex + i];
                } else {
                    cpu.IR[i] = '\0';
                    break;
                }
            } else {
                cpu.IR[i] = '\0';
                break;
            }
        }

        if (strlen(cpu.IR) > 1) {
            words[0] = strtok(cpu.IR, " ");        
            words[1] = strtok(NULL, " ");
            words[2] = strtok(NULL, " ");
            words[3] = strtok(NULL, "");

            errCode = interpret(words);
            printError(errCode);
        }
 

        strcpy(cpu.IR, ""); // reset cpu.IR

        cpu.offset++;
        quanta--;
    }

    return cpu.IP + cpu.offset;
}