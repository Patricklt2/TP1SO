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
#include <time.h>

#define ID_LEN 13

typedef struct memoryCDT{
    char fileID[ID_LEN];
    int flag;
    sem_t sem;
    char map[MEMORY_SIZE];
} memoryCDT;


int _openMem(char* id, int oflag, mode_t mode);
void _trunMem(int fd);
void _randomID(char* buffer);
memoryADT _mapMem(int fd);

memoryADT createSharedMem() {
    memoryADT m;
    char id[] = "placeholder";
    _randomID(id);
    int fd = _openMem(id, O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    _trunMem(fd);
    m = _mapMem(fd);
    strcpy(m->fileID, id);

    if(sem_init(&m->sem, 1, 0) == -1){
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    m->flag = 0;
    return m;
}

memoryADT openExistingMemory(char* id) {
    memoryADT m;
    int fd = _openMem(id, O_RDWR, S_IRUSR | S_IWUSR);
    m = _mapMem(fd);
    return m;
}

void unlinkMemory(memoryADT m) {
    shm_unlink(m->fileID);
}

void setFlag(memoryADT memory, int value) {
    memory->flag = value;
}

int getFlag(memoryADT memory) {
    return memory->flag;
}

sem_t* getMemorySem(memoryADT memory) {
    return &memory->sem;
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
    if(strlen(id) > ID_LEN) {
        perror("invalid mem id length\n");
        exit(1);
    }
    char idAux[ID_LEN + 1];
    idAux[0] = '/';
    strcpy(idAux + 1, id);
    int fd = shm_open(idAux, oflag, mode);
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
    memoryADT aux = mmap(NULL, sizeof(memoryCDT), PROT_WRITE, MAP_SHARED, fd, 0);
    if(aux == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    return aux;
}

void _randomID(char* buffer) {
    srand(time(NULL));
    char charSet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
    int setSize = strlen(charSet);

    int idx;
    int i=0;
    for(; i<ID_LEN - 1; i++) {
        idx = rand() % setSize;
        buffer[i] = charSet[idx];
    }
    buffer[i] = '\0';
}
