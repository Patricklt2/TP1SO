#define _POSIX_SOURCE
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
#include <fcntl.h>
#include <sys/select.h>
#include <signal.h>

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

    int slavesNum = calculateSlavesNum(argc);
    pipechannels pipes[slavesNum];

    for(int i=0;i<slavesNum;i++){
        if(pipe(pipes[i].master_a_slave)==-1||pipe(pipes[i].slave_a_master)==-1){
            perror("pipe machine broke\n");
            exit(1);
        }
        if ((pipes[i].pid = fork()) == 0) {
            if(i > 0){
                int j = i - 1;
                while(j >= 0) {
                    close(pipes[j].master_a_slave[1]);
                    close(pipes[j].slave_a_master[0]);
                    --j;
                }
            }
            createSlave(pipes[i].master_a_slave[1],pipes[i].slave_a_master[0],pipes[i].slave_a_master[1],pipes[i].master_a_slave[0]);
         }
            close(pipes[i].master_a_slave[0]);
            close(pipes[i].slave_a_master[1]);
    }


    memoryADT mem = createSharedMem();
    if(mem == NULL) {
        perror("failed to create shared memory");
        closePipes(pipes, slavesNum);
        return 1;
    }

    write(STDOUT_FILENO, getMemoryID(mem), strlen(getMemoryID(mem)));
    char* memMap = getMemoryMap(mem);
    vistaSem = getMemorySem(mem);


    if(processFiles(pipes, slavesNum, memMap, argc, argv, vistaSem) == -1) {
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
    int i=1;                  //Contador de archivos pasados
    int processed_files = 0;  //Contador de los archivos actualmente procesados

    fd_set read_fds;
    int max_fd = -1;

    //Arranco el ciclo
    if(slavesNum <= numFiles) {
        while(i-1 < slavesNum){
                write(pipes[i-1].master_a_slave[1], files[i], strlen(files[i]));
                i++;
        }
    }

    //Ciclo principal
    while (i < numFiles) {
        // Set de fd
        FD_ZERO(&read_fds);

        for (int j = 0; j < slavesNum; j++) {
            FD_SET(pipes[j].slave_a_master[0], &read_fds);
            max_fd = (pipes[j].slave_a_master[0] > max_fd) ? pipes[j].slave_a_master[0] : max_fd;
        }

        int select_result = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (select_result < 0) {
            perror("select");
            return -1;
        } else if (select_result == 0) {
            // No hay nada listo, sigo
            continue;
        } else {

            for (int j = 0; j < slavesNum && i < numFiles; j++) {
                if (FD_ISSET(pipes[j].slave_a_master[0], &read_fds)) {
                    ssize_t len = read(pipes[j].slave_a_master[0], buffWrite, sizeof(buffWrite));

                    if (len < 0) {
                        perror("read");
                        return -1;
                    }

                    buffWrite[len] = '\0';
                    ptr = buffToMem(ptr, buffWrite, sem);
                    write(pipes[j].master_a_slave[1], files[i], strlen(files[i]));
                    i++;
                    processed_files++;
                }
            }
        }
    }

    //Si quedo algun archivo pendiente, lo levanto
    i = 0;
    while(processed_files < numFiles-1) {
            ssize_t len = read(pipes[i].slave_a_master[0], buffWrite, sizeof(buffWrite));

            if (len < 0) {
                perror("read");
                return -1;
            }

            buffWrite[len] = '\0';
            ptr = buffToMem(ptr, buffWrite, sem);
            processed_files++;
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

double log10(double x) {
    if (x <= 0.0) {
        return -1.0;
    }

    double result = 0.0;
    while (x >= 10.0) {
        x /= 10.0;
        result += 1.0;
    }

    double fractional = 0.5;
    for (int i = 0; i < 15; i++) {
        fractional /= 10.0;
        if (x >= 10.0) {
            while (x >= 10.0) {
                x /= 10.0;
                result += fractional;
            }
        }
    }

    return result;
}

double ceil(double x) {
    int intPart = (int)x;
    if (x > intPart) {
        return intPart + 1.0;
    } else {
        return x;
    }
}

int calculateSlavesNum(int fAmount) {
    return (int)ceil(log10(fAmount))+1;
}

void createSlave(int fd_ms1, int fd_sm0, int fd_out, int fd_in){
    char * argv[] ={"./slave.out",NULL,NULL};

    close(fd_ms1);
    close(fd_sm0);
    close(STDOUT_FILENO);
    dup(fd_out);
    close(STDIN_FILENO);
    dup(fd_in);

    //Cerrar los pipes originales pues ahora estan en STDIN y STDOUT con fd 0 y 1
    close(fd_out);
    close(fd_in);
    execve("./slave.out",argv,NULL);

    return;
}





