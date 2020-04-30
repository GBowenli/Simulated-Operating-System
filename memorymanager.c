#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "ram.h"
#include "pcb.h"
#include "kernel.h"

int PID = 0;
char script[40][1000];

int countTotalPages(FILE *f) { // need to add -lm to end of gcc to do ciel()
    char ch;
    int totalLines = 0;

    ch = fgetc(f);
    while (ch != EOF) {
        if (ch == '\n') {
            totalLines++;
        }
        ch = fgetc(f);
    }   
    return ceil(totalLines/4.0); 
}

void loadPage(int pageNumber, FILE *f, int frameNumber) {
    char ch = ' ';
    int currentLine = 0;
    int startLine = pageNumber * 4;

    rewind(f); // rewind f
    
    while (currentLine < startLine) { // find the beginning of the desired page
        ch = fgetc(f);
        if (ch == '\n') {
            currentLine++;
        }
    }

    for (int i = 0; i < 4; i++) { // concatenate 4 lines of code to script
        while (ch != EOF) {
            ch = fgetc(f);
            strncat(script[frameNumber], &ch, 1);
            if (ch == '\n') {
                break;
            }
        }
    }   
    ram[frameNumber] = script[frameNumber];
}

int findFrame() {
    for (int i = 0; i < 40; i++) {
        if (ram[i] == NULL) {
            return i;
        }
    }
    return -1;
}

int findVictim(struct PCB *p) {
    int randFrame = rand() % 40; // random number 0 to 39
    int frameBelongsToPT = 0;

    for (int i = 0; i < 10; i++) { // check if the random frame number belongs to the page table of the PCB
        if (p->pageTable[i] == randFrame) {
            frameBelongsToPT = 1;
            break;
        }
    }

    if (frameBelongsToPT == 0) {
        return randFrame;
    } else {
        while (frameBelongsToPT != 0) { // find a frame that does not belong to PCB's pages
            randFrame++;
            frameBelongsToPT = 0;

            for (int i = 0; i < 10; i++) { // check if the random frame number belongs to the page table of the pcb
                if (p->pageTable[i] == randFrame) {
                    frameBelongsToPT = 1;
                    break;
                }
            }
        }

        return randFrame;
    }
}

int updatePageTable(struct PCB *p, int pageNumber, int frameNumber, int victimFrame) {
    if (victimFrame == -1) {
        p->pageTable[pageNumber] = frameNumber;
    } else {
        p->pageTable[pageNumber] = victimFrame;
    }

    return 0;
}

int launcher(FILE *p) {
    if (p == NULL) {
        return 0;
    } else {
        char ch;
        char copiedFileTitle[40];
        FILE* newFile;

        sprintf(copiedFileTitle, "./BackingStore/%d.txt", PID);

        newFile = fopen(copiedFileTitle, "w"); // open file in BackingStore in writting mode
        
        while ((ch = fgetc(p)) != EOF) {
            fputc(ch, newFile);
        }

        fclose(p); // close pointer to original file

        rewind(newFile); // rewind the newFile to beginning

        FILE* newFileRead = fopen(copiedFileTitle, "r"); // open the file in reading mode

        int totalPages = countTotalPages(newFileRead);
        int frame;

        myInit(copiedFileTitle, PID, totalPages); // call myInit to create PCB

        if (totalPages == 1) { // load 1 page
            frame = findFrame();
            if (frame != -1) {
                loadPage(0, newFileRead, frame);
                updatePageTable(getPCB(PID), 0, frame, -1);
            } else {
                frame = findVictim(getPCB(PID));
                loadPage(0, newFileRead, frame);
                updatePageTable(getPCB(PID), 0, -1, frame);
            }
        } else { // load 2 pages
            frame = findFrame(); // load first page
            if (frame != -1) {
                loadPage(0, newFileRead, frame);
                updatePageTable(getPCB(PID), 0, frame, -1);
            } else {
                frame = findVictim(getPCB(PID));
                loadPage(0, newFileRead, frame);
                updatePageTable(getPCB(PID), 0, -1, frame);
            }

            frame = findFrame(); // load second page
            if (frame != -1) {
                loadPage(1, newFileRead, frame);
                updatePageTable(getPCB(PID), 1, frame, -1);
            } else {
                frame = findVictim(getPCB(PID));
                loadPage(1, newFileRead, frame);
                updatePageTable(getPCB(PID), 1, -1, frame);
            }
        }

        fclose(newFileRead);

        PID++; // increment fileIndex

        return 1;
    }
}

int pageFault(struct PCB* p) {
    p->PC_page++;

    if (p->PC_page >= p->pages_max) {
        return 0;
    } else {
        char tempScript[1000];
        strcpy(tempScript, "");

        if (p->pageTable[p->PC_page] != -1) { // if pageTable[nextPage] is valid
            p->PC = p->pageTable[p->PC_page];
            p->PC_offset = 0;
        } else {
            char fileTitle[40];
            FILE* file;

            sprintf(fileTitle, "./BackingStore/%d.txt", p->PID);

            file = fopen(fileTitle, "r"); // open file in BackingStore in reading mode

            char ch = ' ';
            int currentLine = 0;
            int startLine = p->PC_page * 4;
            
            int frameNum = findFrame();

            while (currentLine < startLine) { // find the beginning of the desired page
                ch = fgetc(file);
                if (ch == '\n') {
                    currentLine++;
                }
            } 

            for (int i = 0; i < 4; i++) { // concatenate 4 lines of code to script
                while (ch != EOF) {
                    ch = fgetc(file);
                    strncat(tempScript, &ch, 1);
                    if (ch == '\n') {
                        break;
                    }
                }
            }   

            rewind(file);

            if (frameNum != -1) { // there is space on ram
                updatePageTable(p, p->PC_page, frameNum, -1);
                strcpy(script[frameNum], tempScript);
                ram[frameNum] = script[frameNum];
                p->PC = frameNum;
                p->PC_offset = 0;
            } else {
                frameNum = findVictim(p);
                
                strcpy(script[frameNum], tempScript);
                updatePageTable(p, p->PC_page, -1, frameNum);
                ram[frameNum] = script[frameNum];
                p->PC = frameNum;
                p->PC_offset = 0;
            }
        }
        return 1;
    }
}