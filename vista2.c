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
#define NAMED_PIPE "/tmp/myfifo"

void writeOutput(int pipe);
int connectToPipe();


int main(int argc, char* argv[]) {

    int pipe = open(NAMED_PIPE, O_RDONLY);
    if(pipe == -1) {
        return 1;
    }
    writeOutput(pipe);
    close(pipe);

    return 0;
}

void writeOutput(int pipe) {
    char buffer[BUFFER_SIZE];

    while(1) {
        size_t size;
        if((size = read(pipe, buffer, BUFFER_SIZE)) == -1) {
            return;
        }
        buffer[size] = '\0';
        printf("%s", buffer);
    }

}
