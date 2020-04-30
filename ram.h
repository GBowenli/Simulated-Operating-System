extern int ramOverflow;
extern char* ram[40];
void clearRam();
char* getRam(int index);
int getAvailableIndex();
void addToRam(FILE *p, int *start, int *end);