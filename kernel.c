#include<stdlib.h>
#include<stdio.h>
#include "shell.h"
#include "ram.h"
#include "pcb.h"
#include "cpu.h"
#include "memorymanager.h"

struct PCB *head, *tail;

struct PCB* getPCB(int PID) {
    struct PCB *temp = head;

    while (temp->PID != PID) {
        temp = temp->next;
    }

    return temp;
}

void resetReadyQueue() {
    head = NULL;
    tail = NULL;
}  

void addToReady(struct PCB *pcb) {
    if (head == NULL) {
        head = pcb;
    } else if (tail == NULL) {
        tail = pcb;
        head->next = tail;
    } else {
        tail->next = pcb;
        tail = tail->next;
    }
}

void myInit(char *filename, int PID, int totalPages) {
    FILE *file = fopen(filename, "r");
    
    if (file) {
        int start = getAvailableIndex();
        int end = 0;
        addToRam(file, &start, &end);
        
        struct PCB *pcb = makePCB(PID, start, end, totalPages);
        printf("Created new PCB with start: %d, end: %d\n", start, end);

        addToReady(pcb);
    }
}

void scheduler() {
    while(head != NULL) {
        if (cpuAvailable() == 1) {
            setIP(head->pageTable[head->PC_page]);
            setOffset(head->PC_offset);

            head->PC = run(2); // run for 2 quanta

            head->PC_offset = head->PC_offset + 2;

            if (head->PC_offset < 4) {
                struct PCB* newPCB = (struct PCB*)malloc(sizeof(struct PCB));

                newPCB->PID = head->PID;
                newPCB->PC = head->PC;
                newPCB->PC_page = head->PC_page;
                newPCB->PC_offset = head->PC_offset;
                newPCB->pages_max = head->pages_max;
                newPCB->start = head->start;
                newPCB->end = head->end;
                for (int i = 0; i < 10; i++) {
                    newPCB->pageTable[i] = head->pageTable[i];
                }

                if (tail == NULL) {
                    head->next = newPCB;
                } else {
                    tail->next = newPCB;
                    tail = tail->next;
                }
            } else if (head->PC_offset == 4) {
                int hasMorePages = pageFault(head);

                if (hasMorePages == 0) { // remove file if it does not have anymore pages
                    char removeCommand[40];
                    sprintf(removeCommand, "rm -r ./BackingStore/%d.txt", head->PID);

                    system(removeCommand);
                } else {
                    struct PCB* newPCB = (struct PCB*)malloc(sizeof(struct PCB));

                    newPCB->PID = head->PID;
                    newPCB->PC = head->PC;
                    newPCB->PC_page = head->PC_page;
                    newPCB->PC_offset = head->PC_offset;
                    newPCB->pages_max = head->pages_max;
                    newPCB->start = head->start;
                    newPCB->end = head->end;
                    for (int i = 0; i < 10; i++) {
                        newPCB->pageTable[i] = head->pageTable[i];
                    }

                    if (tail == NULL) {
                        head->next = newPCB;
                    } else {
                        tail->next = newPCB;
                        tail = tail->next;
                    }                
                }
            }
            head = head->next;
        }
    }
}

int kernel() {
    printf("Kernal 2.0 loaded!\n");
    shellUI();
    return 0;
}

void boot() {
    for (int i = 0; i < 40; i++) {
        ram[i] = NULL;
    }
    system("rm -r BackingStore");
    system("mkdir BackingStore");
}

int main() {
    int error = 0;
    boot();
    error = kernel();
    return error;
}