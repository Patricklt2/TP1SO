#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, const char * argv[]) {
    // comando para calcular el md5
    char *command = malloc(sizeof(char)*100);
    strcat(command,"md5sum ");
    strcat(command, argv[0]);

    // Abrir pipe para captar el call
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error executing command");
        return 1;
    }

    // Leo y cargo el hash
    char buffer[128];
    fgets(buffer, sizeof(buffer), fp);

    printf("%s",buffer);


    // Cierro pipe
    pclose(fp);
    free(command);

    return 0;
}
