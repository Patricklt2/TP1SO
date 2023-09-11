#ifndef TP1SO_MEMORYADT_H
#define TP1SO_MEMORYADT_H

#include <semaphore.h>
#define MEMORY_SIZE 1048576

typedef struct memoryCDT* memoryADT;

/**
 * creates a new shared memory address
 * srand() should be called before executing this command
 * @return ADT
 */
memoryADT createSharedMem();

/**
 * opens an existing shared memory address
 * @param id a 12 character long string
 * @return ADT if successful, NULL if not
 */
memoryADT openExistingMemory(char* id);

/**
 * unlinks the shared memory address
 * @param m ADT
 */
void unlinkMemory(memoryADT m);

/**
 * sets the flag parameter in the ADT to a desired value
 * @param memory
 * @param value
 */
void setFlag(memoryADT memory, int value);

/**
 * retrieves the flag value
 * @param memory
 * @return flag value
 */
int getFlag(memoryADT memory);

/**
 * returns a pointer to a char array with size defined by MEMORY_SIZE
 * @param memory
 * @return
 */
char* getMemoryMap(memoryADT memory);

/**
 * returns the 12 character string identifier
 * @param memory
 * @return
 */
char* getMemoryID(memoryADT memory);

/**
 * returns a pointer to the shared memory semaphore
 * @param memory
 * @return
 */
sem_t* getMemorySem(memoryADT memory);

#endif //TP1SO_MEMORYADT_H
