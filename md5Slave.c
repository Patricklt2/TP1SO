#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BYTES_MD5HASH 33 //32 para el hash + 1 para el  '\0'

int main(int argc, const char * argv[]) {
    // comando para calcular el md5
    char *command = malloc(sizeof(char)*100);
    strcpy(command,"md5sum ");
    strcat(command, argv[0]);

    // Abrir pipe para captar el call
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error executing command");
        return 1;
    }

    // Leo y cargo el hash
    char buffer[BYTES_MD5HASH];
    fgets(buffer, sizeof(buffer), fp);

    printf("%s %s",buffer, argv[0]);


    // Cierro pipe
    pclose(fp);
    free(command);

    return 0;
}
