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

int main(int argc, char* argv[]) {
    char appOutput[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    if(isatty(fileno(stdin))) {
        if(argc > 1) {
            if(getMemoryAddress(argv[1], appOutput) == -1) {
                return 1;
            }
        } else {
            perror("The vista process was not given any processes to read from\n");
            return 1;
        }
    } else {
        fgets(appOutput, BUFFER_SIZE, stdin);
    }


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



int getMemoryAddress(char* pName, char* buffer) {
    int appPid;
    char command[BUFFER_SIZE];
    sprintf(command, "pgrep %s", pName);

    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen");
        return -1;
    }

    if (fscanf(fp, "%d", &appPid) != 1) {
        fprintf(stderr, "Couldn't find a process related to %s\n", pName);
        pclose(fp);
        return -1;
    }

    //code taken from https://www.commandlinefu.com/commands/view/5410/intercept-stdoutstderr-of-another-process
    //works by intercepting the stdout of a process by using the pid
    sprintf(command,
            "strace -ff -e write=1,2 -s 1024 -p %d 2>&1 | grep \"^ |\" | cut -c11-60 | sed -e 's/ //g' | xxd -r -p",
            appPid);

    FILE *input = popen(command, "r");
    fgets(buffer, BUFFER_SIZE, input);
    pclose(input);
    pclose(fp);
    return 0;
}
