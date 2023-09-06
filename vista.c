#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include "memoryADT.h"

int main(int argc, char* argv[]) {
    char appOutput[50];
    char buffer[128];
    int semVal;
    fgets(appOutput, 50, stdin);

    memoryADT sharedMem = openExistingMemory(appOutput);
    char* memMap = getMemoryMap(sharedMem);
    char* mapPtr = memMap;

    sem_t* appSem = getMemorySem(sharedMem);

    //TODO sacar esto, sirve de ejemplo para mostrar como se puede escribir y leer en memoria compartida

    for(int i=0; i<2; i++) {
        sem_wait(appSem);
        strcpy(buffer, mapPtr);
        printf("%s", buffer);
        mapPtr += strlen(buffer) + 1;
    }

    return 0;
}