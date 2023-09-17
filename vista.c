#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include "memoryADT.h"

#define BUFFER_SIZE 128

int getMemoryAddress(char* pName, char* buffer);
void writeOutput(memoryADT mem);
int connectToApp(char* buffer);

int main(int argc, char* argv[]) {
    char* appOutput;

    if(argc == 2) {
        appOutput = argv[1];
    }
    else {
        char aux[BUFFER_SIZE];
        appOutput = aux;
        if(connectToApp(appOutput) == -1) {
            perror("the vista process failed to connect to the application process\n");
            return 1;
        }
    }

    memoryADT sharedMem = openExistingMemory(appOutput);
    if(sharedMem == NULL) {
        perror("failed to open shared memory");
        return 1;
    }

    writeOutput(sharedMem);
    unlinkMemory(sharedMem);
    return 0;
}

int connectToApp(char* buffer) {
    if (!isatty(STDIN_FILENO)) {
        read(STDIN_FILENO, buffer, BUFFER_SIZE);
        return 0;
    }
    return -1;
}

void writeOutput(memoryADT mem) {
    char buffer[BUFFER_SIZE];
    char* memMap = getMemoryMap(mem);
    char* mapPtr = memMap;
    sem_t* appSem = getMemorySem(mem);
    int semValue;

    while(1) {
        if(getFlag(mem) == 1) {
            //this is not for busy waiting, but instead to prevent the vista process from
            //waiting on an unlinked semaphore
            if(sem_getvalue(appSem, &semValue) == -1) {
                perror("sem_getvalue");
                exit(EXIT_FAILURE);
            }

            if(semValue == 0){
                return;
            }
        }
        sem_wait(appSem);
        strcpy(buffer, mapPtr);
        printf("%s", buffer);
        mapPtr += strlen(buffer) + 1;
    }
}
