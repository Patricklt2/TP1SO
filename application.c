#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>
#include "memoryADT.h"
#include "publicInfo.h"
#include <fcntl.h>
#include <sys/select.h>

#define SLAVENUM 2
#define FILES_PER_SLAVENUM 20
#define MAXFILESPERSLAVE 2

typedef struct pipechannles{
    int pid;
    int slave_a_master[2];
    int master_a_slave[2];
}pipechannels;

int calculateSlavesNum(int fAmount);
void closePipes(pipechannels* pipes, int slavesNum);
int processFiles(pipechannels* pipes, int slavesNum, char* ptr, int numFiles, char* files[], sem_t* sem);
void createSlave(int fd_ms1, int fd_sm0, int fd_out, int fd_in);
void cleanUp(memoryADT mem, pipechannels* pipes, int slavesNum);

int main(int argc, char *argv[]) {

    if(argc<2)
        return 1;

    srand(time(NULL));
    sem_t* vistaSem;

    sem_t* memReadySem = sem_open(MEM_READY_SEM, O_CREAT, S_IRUSR|S_IWUSR, 0);

    if (memReadySem == SEM_FAILED) {
        perror("failed to create semaphore");
        return 1;
    }

    int slavesNum =5;//ver bien de como calcular la cant de slaves
    pipechannels pipes[slavesNum];

    for(int i=0;i<slavesNum;i++){
        if(pipe(pipes[i].master_a_slave)==-1||pipe(pipes[i].slave_a_master)==-1){
            perror("pipe machine broke\n");
            exit(1);
        }
        if ((pipes[i].pid = fork()) == 0) {
            createSlave(pipes[i].master_a_slave[1],pipes[i].slave_a_master[0],pipes[i].slave_a_master[1],pipes[i].master_a_slave[0]);
         }
            close(pipes[i].master_a_slave[0]);
            close(pipes[i].slave_a_master[1]);
    }

    memoryADT mem = createSharedMem();
    if(mem == NULL) {
        perror("failed to create shared memory");
        sem_unlink(MEM_READY_SEM);
        closePipes(pipes, slavesNum);
        return 1;
    }

    write(STDOUT_FILENO, getMemoryID(mem), strlen(getMemoryID(mem)));
    sem_post(memReadySem);

    //prints on stdout the information necessary for the vista process to connect
    char* memMap = getMemoryMap(mem);
    vistaSem = getMemorySem(mem);
    char* mapPtr = memMap;

    if(processFiles(pipes, slavesNum, mapPtr, argc, argv, vistaSem) == -1) {
        cleanUp(mem, pipes, slavesNum);
        perror("an error occurred when processing files");
        return 1;
    }

    cleanUp(mem, pipes, slavesNum);
    return 0;
}


void cleanUp(memoryADT mem, pipechannels* pipes, int slavesNum) {
    setFlag(mem, 1);
    sleep(2);
    sem_unlink(MEM_READY_SEM);
    unlinkMemory(mem);
    closePipes(pipes, slavesNum);
}

char* buffToMem(char* ptr, char* buff, sem_t* sem) {
    while(*buff != '\0') {
        *ptr = *buff;
        buff++;
        ptr++;
    }
    sem_post(sem);
    return ++ptr;
}

int processFiles(pipechannels* pipes, int slavesNum, char* ptr, int numFiles, char* files[], sem_t* sem) {
    char buffWrite[256];
    int i=1;

    while(i < numFiles){
        write(pipes[(i-1)%slavesNum].master_a_slave[1],files[i],strlen(files[i]));
        ssize_t len=read(pipes[(i-1)%slavesNum].slave_a_master[0],buffWrite,sizeof(buffWrite));

        if(len<0) return -1;

        buffWrite[len]='\0';
        ptr = buffToMem(ptr, buffWrite, sem);
        i++;
    }
    return 0;
}

void closePipes(pipechannels* pipes, int slavesNum) {
    for(int i=0;i<slavesNum;i++){
        close(pipes[i].slave_a_master[1]);
        close(pipes[i].master_a_slave[0]);
        kill(pipes[i].pid,1);
    }
}

int calculateSlavesNum(int fAmount) {
    return (fAmount < SLAVENUM) ? (fAmount) : (((fAmount / FILES_PER_SLAVENUM) + 1) * SLAVENUM);
}

void createSlave(int fd_ms1, int fd_sm0, int fd_out, int fd_in){
    char * argv[] ={"./slave.out",NULL,NULL};

    close(fd_ms1);
    close(fd_sm0);
    close(STDOUT_FILENO);
    dup(fd_out);
    close(STDIN_FILENO);
    dup(fd_in);
    execve("./slave.out",argv,NULL);

    return;
}



