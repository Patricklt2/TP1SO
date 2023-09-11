#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <libgen.h>
#include <semaphore.h>
#include "memoryADT.h"

#define NUM_CHILD 20
#define TRUE 1
#define FALSE 0
#define BYTES_MD5HASH 33 //32 para el hash + 1 para el  '\0'

int auxMd5(char * fpath, char * extBuff){
    // comando para calcular el md5
    char *command = malloc(sizeof(char)*200);
    strcpy(command,"md5sum ");
    strcat(command, "\"");
    strcat(command, fpath);
    strcat(command, "\"");

    // Abrir pipe para captar el call
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error executing command");
        return 1;
    }

    // Leo y cargo el hash
    char buffer[BYTES_MD5HASH];
    fgets(buffer, sizeof(buffer), fp);

    strcpy(extBuff,buffer);
    strcat(extBuff, " ");
    strcat(extBuff,basename(fpath));

    // Cierro pipe
    pclose(fp);
    free(command);

    return 0;
}

int main(int argc,char* argv[]){
    int i = 0;
    if(argc!=2)
        exit(1);
    memoryADT memRead=openExistingMemory(argv[0]);
    sem_t* semRead=getMemorySem(memRead);
    memoryADT memWrite=openExistingMemory(argv[1]);
    sem_t* semWrite=getMemorySem(memWrite);
    char bufferWrite[200];  //Write to parent
    char bufferRead[200];   //Read from parent
    char aux[200];
    while(1){
        sem_post(semWrite);
        ssize_t bytes_r = read(STDIN_FILENO,bufferRead,sizeof(bufferRead));
        if(bytes_r<0){printf("reoto todo\n");exit(1);}
        bufferRead[bytes_r] = '\0';
        auxMd5(bufferRead,bufferWrite);
        if(i == NUM_CHILD)  //Para ese entonces el proceso hijo 0 ya va a haber terminado, entonces puedo rehusar el fd
            i = 0;
        sem_wait(semRead);
        sprintf(aux,"PID: %d %s\n", getpid(), bufferWrite);
        write(STDOUT_FILENO,aux,strlen(aux));

    }
    exit(0);
}