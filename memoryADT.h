#ifndef TP1SO_MEMORYADT_H
#define TP1SO_MEMORYADT_H

#define MEMORY_SIZE 1048576

typedef struct memoryCDT* memoryADT;

memoryADT createSharedMem();

memoryADT openExistingMemory(char* id);

char* getMemoryMap(memoryADT memory);

char* getMemoryID(memoryADT memory);

void freeMem(memoryADT memory);


#endif //TP1SO_MEMORYADT_H
