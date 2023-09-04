#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "memoryADT.h"

int main(int argc, char* argv[]) {
    char appOutput[50];

    fgets(appOutput, 50, stdin);
    memoryADT sharedMem = openExistingMemory(appOutput);
    char* memMap = getMemoryMap(sharedMem);

    printf("%s", memMap);
    return 0;
}