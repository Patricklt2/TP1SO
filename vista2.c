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
#define NAMED_PIPE "pipe"

void writeOutput(int pipe);
int connectToPipe();


int main(int argc, char* argv[]) {

    int pipe = open(NAMED_PIPE, O_RDONLY);
    if(pipe == -1) {
        return 1;
    }
    writeOutput(sharedMem);
    close(pipe);

    return 0;
}

void writeOutput(int pipe) {
    char buffer[BUFFER_SIZE];

    while(1) {
        if(read(pipe, buffer) == -1) {
            return;
        }
        printf("%s", buffer);
    }

}
