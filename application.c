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


//TODO agregar una funcion de exit global
int main(int argc, char *argv[]) {
    srand(time(NULL));
    if(argc<2)
        exit(1);
    sem_t* vistaSem;

    FILE* fp;
    if(fp=fopen("archivo", "w"));

    sem_t* memReadySem = sem_open(MEM_READY_SEM, O_CREAT, S_IRUSR|S_IWUSR, 0);

    if (memReadySem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }



    int slavesNum =5;//ver bien de como calcular la cant de slaves
    pipechannels pipes[slavesNum];
    for(int i=0;i<slavesNum;i++){
        if(pipe(pipes[i].master_a_slave)==-1||pipe(pipes[i].slave_a_master)==-1){
            perror("pipe machine broke\n");
            exit(1);
        }
        if ((pipes[i].pid = fork()) == 0) {
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
    if(mem == NULL) {
        //TODO salir de una manera mas prolija, que no deje al vista bloqueado
        perror("failed to create shared memory");
        exit(1);
    }

    write(STDOUT_FILENO, getMemoryID(mem), strlen(getMemoryID(mem)));
    sem_post(memReadySem);

    //prints on stdout the information necessary for the vista process to connect
    char* memMap = getMemoryMap(mem);
    vistaSem = getMemorySem(mem);
    char* mapPtr = memMap;


    //Sincronizar Padre e Hijos -- Los hijos van a usar semaforos internamente
    fd_set read_fds, write_fds;
    int max_fd_read = -1;
    int max_fd_write = -1;

    for (int i = 0; i < slavesNum; i++) {
        if (pipes[i].slave_a_master[0] > max_fd_read) {
            max_fd_read = pipes[i].slave_a_master[0];
        }
        if (pipes[i].master_a_slave[1] > max_fd_write) {
            max_fd_write = pipes[i].master_a_slave[1];
        }
    }


    char buffWrite[128];
    int i=1;
    while(i < argc){

        //Se limpian cada iteracion
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);

        // Se agregan cada iteracion
        for (int j = 0; j < slavesNum; j++) {
            FD_SET(pipes[j].slave_a_master[0], &read_fds);
        }

        for (int j = 0; j < slavesNum; j++) {
            FD_SET(pipes[j].master_a_slave[1], &write_fds);
        }

        // Verifico si hay algun pipe listo para ser escrito
        int wReady_fds = select(max_fd_write + 1, NULL, &write_fds, NULL, NULL);

        if (wReady_fds == -1) {
            perror("select");
            exit(1);
        }

        for (int j = 0; j < slavesNum; j++) {
            if (FD_ISSET(pipes[j].master_a_slave[1], &write_fds) && i < argc) {
                write(pipes[j].master_a_slave[1], argv[i], strlen(argv[i]));
                i++;
            }
        }


        int rReady_fds = select(max_fd_read + 1, &read_fds, NULL, NULL, NULL);

        if (rReady_fds == -1) {
            perror("select");
            exit(1);
        }

        for (int j = 0; j < slavesNum; j++) {
            if (FD_ISSET(pipes[j].slave_a_master[0], &read_fds)) {
                // Read data from the selected pipe.
                ssize_t len = read(pipes[j].slave_a_master[0], buffWrite, sizeof(buffWrite));
                if(len<0){printf("error en read\n");exit(1);}
                buffWrite[len] = '\0';
                fprintf(fp, "%s\n", buffWrite);
                // Notify Vista process or perform other actions as needed
                strcpy(mapPtr, buffWrite);
                sem_post(vistaSem);
                mapPtr += strlen(buffWrite) + 1;

            }
        }
    }

    fclose(fp);

    //decides how many slave processes need to be created and initializes them

    //waits until a slave process finishes, and writes the result on the buffer

    //assigns a new file to the slave process, if there's none left, the slave is killed

    //once all the slaves finish, writes to the result file and returns

    setFlag(mem, 1);

    sleep(2);
    sem_unlink(MEM_READY_SEM);
    unlinkMemory(mem);

    for(int i=0;i<slavesNum;i++){
        close(pipes[i].slave_a_master[1]);
        close(pipes[i].master_a_slave[0]);
        kill(pipes[i].pid,1);
    }
}


int calculateSlavesNum(int fAmount) {
    return (fAmount < SLAVENUM) ? (fAmount) : (((fAmount / FILES_PER_SLAVENUM) + 1) * SLAVENUM);
}



