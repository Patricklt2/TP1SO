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
#include "memoryADT.h"

#define SLAVENUM 5
#define FILES_PER_SLAVENUM 20
#define MAXFILESPERSLAVE 2

typedef struct pipechannles{
    int pid;
    int slave_a_master[2];
    int master_a_slave[2];
}pipechannels;

int calculateSlavesNum(int fAmount);

int main(int argc, char *argv[]) {
    int currentFile=0;
    sem_t* vistaSem;
    int semVal;
    int slavesNum = calculateSlavesNum(argc - 1);
    int vistaRunning = 0;

    pipechannels *pipes = malloc(slavesNum * (sizeof(pipechannels)));
    /*
    for(int i=0;i<slavesNum;i++){
        open(pipes[i].master_a_slave);//todo de ver como manipular los pipes
        open(pipes[i].slave_a_master);
        if((pipes[i].pid=fork())==0){
            close(pipes[i].master_a_slave[1]);
            close(pipes[i].slave_a_master[0]);
            dup2(pipes[i].slave_a_master[1],STDOUT_FILENO);
            dup2(pipes[i].master_a_slave[0],STDIN_FILENO);
            break;
         }
        else{
            close(pipes[i].master_a_slave[0]);
            close(pipes[i].slave_a_master[1]);
            dup2(pipes[i].slave_a_master[0],STDIN_FILENO);
            dup2(pipes[i].master_a_slave[1],STDOUT_FILENO);
        }
    }
*/
    memoryADT mem = createSharedMem();
    //prints on stdout the information necessary for the vista process to connect
    fputs(getMemoryID(mem), stdout);

    sleep(2);
    char* memMap = getMemoryMap(mem);
    vistaSem = getMemorySem(mem);
    char* mapPtr = memMap;


    //TODO sacar esto, sirve de ejemplo para mostrar como se puede escribir y leer en memoria compartida
    char* str1 = "Hello Barbie\n";
    char* str2 = "Hello Ken\n";

    strcpy(mapPtr, str1);
    sem_post(vistaSem);

    mapPtr += strlen(str1) + 1;

    strcpy(mapPtr, str2);
    sem_post(vistaSem);

    //decides how many slave processes need to be created and initializes them

    //waits until a slave process finishes, and writes the result on the buffer

    //assigns a new file to the slave process, if there's none left, the slave is killed

    //once all the slaves finish, writes to the result file and returns
    free(pipes);
}


int calculateSlavesNum(int fAmount) {
    return (fAmount < SLAVENUM) ? (fAmount) : (((fAmount / FILES_PER_SLAVENUM) + 1) * SLAVENUM);
}
