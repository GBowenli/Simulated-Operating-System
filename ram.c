#include<stdlib.h>
#include<stdio.h>
#include<string.h>

char *ram[40];
int ramOverflow = 0;

char* getRam(int index) {
    return ram[index];
}

int getAvailableIndex() {
    int count = 0;

    for (int i = 0; i < 40; i++) {
        if (ram[i] != NULL) {
            count++;
        } else {
            return count;
        }
    }
}

void clearRam() {
    for (int i = 0; i < 40; i++) {
        ram[i] = NULL;
    }
}

void addToRam(FILE *p, int *start, int *end) {
    char line[100];
    int counter = 0;

    while (fgets(line, sizeof(line), p)) {
        if (line[strlen(line)-2] == '\r') { // remove the \r\n in the line
            line[strlen(line)-2] = '\0';
        }

        if (*start + counter == 40) {
            ramOverflow = 1;
            return;
        }
        
        counter++;
    }
    *end = *start+counter-1;
    fclose(p);
}