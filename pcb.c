#include<stdlib.h>
#include<stdio.h>

struct PCB { 
    int PID;
    int PC; 
    int PC_page;
    int PC_offset;
    int pages_max;
    int start; 
    int end;
    int pageTable[10];
    struct PCB* next;
};

struct PCB* makePCB(int PID, int start, int end, int totalPages) {
    struct PCB *newPCB = (struct PCB*) malloc(sizeof(struct PCB));
    newPCB->PID = PID;
    newPCB->PC = start;
    newPCB->PC_page = 0;
    newPCB->PC_offset = 0;
    newPCB->pages_max = totalPages;
    newPCB->start = start;
    newPCB->end = end;
    for (int i = 0; i < 10; i++) { // initialize pageTable as -1   
        newPCB->pageTable[i] = -1;
    }
    newPCB->next = NULL;
    return newPCB;
}
