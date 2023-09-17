#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <libgen.h>
#include "semaphore.h"

#define TRUE 1
#define FALSE 0
#define BYTES_MD5HASH 33 //32 para el hash + 1 para el  '\0'

int auxMd5(char * fpath, char * extBuff){
    // comando para calcular el md5
    char *command = malloc(sizeof(char)*300);
    strcpy(command,"md5sum ");
    strcat(command, "\"");   //Comillas habilitan que el md5 parsee bien files con " " espacios
    strcat(command, fpath);
    strcat(command, "\" 2>/dev/null"); // Redirijo mensaje de error del md5sum para suprimir

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

int main(){
    char bufferWrite[200];  //Write to parent
    char bufferRead[256];   //Read from parent
    char aux[256];

    while(TRUE){
        ssize_t bytes_r = read(STDIN_FILENO,bufferRead,sizeof(bufferRead));

        if(bytes_r<0){printf("roto todo\n");exit(1);}

        bufferRead[bytes_r] = '\0';
        auxMd5(bufferRead,bufferWrite);

        sprintf(aux,"PID: %d %s\n",getpid(), bufferWrite);
        write(STDOUT_FILENO,aux,strlen(aux));
    }


    exit(0);
}
