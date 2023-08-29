#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <openssl/md5.h>
#include <unistd.h>
#include <string.h>

//Funcion aux para calcular el md5sum y devolverlo en formato string
int calculate_md5sum(char *filename, char* str)
{
    //Abre el archivo y verifica que no sea nulo
    FILE *file_ptr;
    file_ptr = fopen(filename, "r");
    if (file_ptr==NULL)
    {
        perror("Error opening file");
        fflush(stdout);
        return 1;
    }

    MD5_CTX c;
    char buf[512]; //Buffer de lectura
    ssize_t bytes;
    unsigned char out[MD5_DIGEST_LENGTH]; //Se guarda el hash aca

    //Se inicializa el struct
    MD5_Init(&c);

    //Loop que genera y actualiza el hash
    do
    {
        bytes=fread(buf, 1, 512, file_ptr);
        MD5_Update(&c, buf, bytes);
    }while(bytes > 0);

    //En base a los bytes leidos y guardados en c se genera el hash final
    MD5_Final(out, &c);

    int pid = getpid();



    strcat(str,"MD5: ");

    //Puntero auxiliar para copiar hash al puntero string y poder concatenar el HASH
    char *ptr = str;
    ptr+= 5; //Offset del "MD5: " que son 5 bytes

    for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
        sprintf(ptr, "%02x", out[i]);
        ptr+=2;
    }

    //idem pero para el PID
    char * auxPid = malloc(sizeof(int)*2);
    sprintf(auxPid,"%d",pid);

    //Hacer mas eficiente
    strcat(str, " | File: ");
    strcat(str, filename);
    strcat(str, " | Pid: ");
    strcat(str, auxPid);

    free(auxPid);

    return 0;
}

//Falta hacer que main tome el argv[1] via pipe y que en vez de printf, mande el string con all via pipe al padre
//COMPILAR SLAVE CON -lssl -lcrypto -Wall -pedantic
int main(int argc, char *argv[])
{
    char *str = malloc(sizeof(char)*100);
    calculate_md5sum(argv[1], str);

    printf("%s\n",str);
    free(str);
}
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_CHILD 3

void child_process(int pipe_fd_read, int pipe_fd_write, const char * fPath) {
    // Cierro pipe de lectura
    close(pipe_fd_read);

    // Redirijo la salida estandard del hijo
    dup2(pipe_fd_write, STDOUT_FILENO);

    // Proceso nuevo
    char * const argv[] = {fPath,NULL};  // Le paso el path al hijo
    char * const envp[] = {NULL};
    execve("./md5Slave", argv, envp);

    perror("Error executing child program");
    exit(1);
}

int main() {
    pid_t child_pids[NUM_CHILD];
    int pipe_fds[NUM_CHILD][2];  // Array de fd's

    const char *fPaths[NUM_CHILD] = {
            "./README.txt",
            "./vista.c",
            "./application.c"
    };  // Paths to files

    for (int i = 0; i < NUM_CHILD; i++) {
        if (pipe(pipe_fds[i]) == -1) {
            perror("Error creating pipe");
            return 1;
        }

        child_pids[i] = fork();

        if (child_pids[i] == -1) {
            perror("Error creating child process");
            return 1;
        } else if (child_pids[i] == 0) {  // hijo
            child_process(pipe_fds[i][0], pipe_fds[i][1], fPaths[i]);
            exit(0);
        } else {  // padre

        }
    }

    // Padre sigue // levanto los datos
        char buff[200];
    for (int i = 0; i < NUM_CHILD; i++) {
        read(pipe_fds[i][0],buff,sizeof(buff));
        printf("%s",buff);
    }


    return 0;
}