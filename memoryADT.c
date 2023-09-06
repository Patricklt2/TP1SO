#include "memoryADT.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <semaphore.h>
#include <string.h>

typedef struct memoryCDT{
    char fileID[64];
    sem_t sem;
    char map[MEMORY_SIZE];
} memoryCDT;


int _openMem(char* id, int oflag, mode_t mode);
void _trunMem(int fd);
memoryADT _mapMem(int fd);

memoryADT createSharedMem() {
    //TODO generar nombre aleatorio
    memoryADT m;
    char* id = "/placeholder";
    int fd = _openMem(id, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    _trunMem(fd);
    m = _mapMem(fd);
    strcpy(m->fileID, id);
    return m;
}

memoryADT openExistingMemory(char* id) {
    memoryADT m;
    int fd = _openMem(id, O_RDWR, S_IRUSR | S_IWUSR);
    m = _mapMem(fd);
    return m;
}

char* getMemoryMap(memoryADT memory) {
    return memory->map;
}

char* getMemoryID(memoryADT memory) {
    return memory->fileID;
}


//AUXILIARY FUNCTIONS
//TODO manejar los errores mejor
int _openMem(char* id, int oflag, mode_t mode) {
    int fd = shm_open(id, oflag, mode);
    if(fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    return fd;
}
void _trunMem(int fd) {
    if(ftruncate(fd, sizeof(memoryCDT)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
}
memoryADT _mapMem(int fd) {
    char* aux = mmap(NULL, sizeof(memoryCDT), PROT_WRITE, MAP_SHARED, fd, 0);
    if(aux == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    return aux;
}
