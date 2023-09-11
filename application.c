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
#include "queuefile.h"

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

    if(argc<2)
        exit(1);
    sem_t* vistaSem;

    Fqueue q = newQueue();

    for(int i=1;i<argc;i++){//encola todas las files al principio, ahorrandonos problemas de sincronizacion (espero)
        enqueue(q,argv[i]);
    }

    int slavesNum = calculateSlavesNum(argc-2);//ver bien de como calcular la cant de slaves
    pipechannels pipes[slavesNum];
    for(int i=0;i<slavesNum;i++){
        if(pipe(pipes[i].master_a_slave)==-1||pipe(pipes[i].slave_a_master)==-1){
            perror("pipe machine broke\n");
            exit(1);
        }
        if((pipes[i].pid=fork())==0){
            close(pipes[i].master_a_slave[1]);
            close(pipes[i].slave_a_master[0]);
            close(STDOUT_FILENO);
            dup(pipes[i].slave_a_master[1]);
            close(STDIN_FILENO);
            dup(pipes[i].master_a_slave[0]);
            execve("./slave.out",NULL,NULL);
         }
            close(pipes[i].master_a_slave[0]);
            close(pipes[i].slave_a_master[1]);
    }
    memoryADT mem = createSharedMem();
    fputs(getMemoryID(mem), stdout);

    //prints on stdout the information necessary for the vista process to connect
    char* memMap = getMemoryMap(mem);
    vistaSem = getMemorySem(mem);
    char* mapPtr = memMap;
    char buff[100];
    char buffWrite[100];
    int i=0;
    while(!isempty(q)){
        dequeue(q,buff);
        sleep(1);
        write(pipes[i%slavesNum].master_a_slave[1],buff,strlen(buff));
        ssize_t len=read(pipes[i%slavesNum].slave_a_master[0],buffWrite,sizeof(buffWrite));
        if(len<0){printf("error en read\n");exit(1);}
        buffWrite[len]='\0';
        printf("PID: %s",buffWrite);
        i++;
    }
    free(q);



    //TODO sacar esto, sirve de ejemplo para mostrar como se puede escribir y leer en memoria compartida
    //de paso ver como hacer para que
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

    setFlag(mem, 1);
    for(int i=0;i<slavesNum;i++){
        close(pipes[i].master_a_slave[1]);
        close(pipes[i].slave_a_master[0]);
        kill(pipes[i].pid,1);
    }

}


int calculateSlavesNum(int fAmount) {
    return (fAmount < SLAVENUM) ? (fAmount) : (((fAmount / FILES_PER_SLAVENUM) + 1) * SLAVENUM);
}



