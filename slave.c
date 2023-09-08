#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <libgen.h>

#define NUM_CHILD 20
#define TRUE 1
#define FALSE 0
#define BYTES_MD5HASH 33 //32 para el hash + 1 para el  '\0'

int auxMd5(char * fpath, char * extBuff){
    // comando para calcular el md5
    char *command = malloc(sizeof(char)*100);
    strcpy(command,"md5sum ");
    strcat(command, fpath);

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
    int i = 0;
    char bufferWrite[128];  //Write to parent
    char bufferRead[128];   //Read from parent

    while(TRUE){
        ssize_t bytes_r = read(STDIN_FILENO,bufferRead,sizeof(bufferRead));
        bufferRead[bytes_r] = '\0';

        auxMd5(bufferRead,bufferWrite);
        if(i == NUM_CHILD)  //Para ese entonces el proceso hijo 0 ya va a haber terminado, entonces puedo rehusar el fd
            i = 0;

        printf("%d %s\n", getpid(), bufferWrite);
    }

    exit(0);
}