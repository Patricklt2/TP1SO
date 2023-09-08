#ifndef TP1SO_MEMORYADT_H
#define TP1SO_MEMORYADT_H

#include <semaphore.h>
#define MEMORY_SIZE 1048576
#define FLAGS_NUM 5

typedef struct memoryCDT* memoryADT;

memoryADT createSharedMem();

memoryADT openExistingMemory(char* id);

void setFlag(memoryADT memory, int value);

int getFlag(memoryADT memory);

char* getMemoryMap(memoryADT memory);

char* getMemoryID(memoryADT memory);

sem_t* getMemorySem(memoryADT memory);

#endif //TP1SO_MEMORYADT_H
