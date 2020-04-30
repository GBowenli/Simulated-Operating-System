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

struct PCB* makePCB(int PID, int start, int end, int totalPages);