#include "memoryADT.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>

typedef struct memoryCDT{
    char* fileID;
    int fd;
    char* map;
} memoryCDT;

typedef memoryCDT* memoryADT;

int _openMem(char* id, int oflag, mode_t mode);
void _trunMem(int fd);
char* _mapMem(int fd);

memoryADT createSharedMem() {
    //TODO generar nombre aleatorio
    memoryADT m = malloc(sizeof(memoryCDT));

    m->fileID = "/placeholder";
    m->fd = _openMem(m->fileID, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    _trunMem(m->fd);
    m->map = _mapMem(m->fd);

    return m;
}

memoryADT openExistingMemory(char* id) {
    memoryADT m = malloc(sizeof(memoryCDT));

    m->fileID = id;
    m->fd = _openMem(m->fileID, O_RDWR, S_IRUSR | S_IWUSR);
    m->map = _mapMem(m->fd);

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
    if(ftruncate(fd, MEMORY_SIZE) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
}
char* _mapMem(int fd) {
    char* aux = mmap(NULL, MEMORY_SIZE, PROT_WRITE, MAP_SHARED, fd, 0);
    if(aux == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    return aux;
}


//FREE

void freeMem(memoryADT m) {
    free(m);
}
