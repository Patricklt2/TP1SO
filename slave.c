#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_CHILD 20
#define TRUE 1
#define FALSE 0

void child_process(int pipe_fd_read, int pipe_fd_write, char * fPath) {
    // Cierro pipe de lectura
    close(pipe_fd_read);

    // Redirijo la salida estandard del hijo
    dup2(pipe_fd_write, STDOUT_FILENO);

    // Proceso nuevo
    char * argv[] = {fPath,NULL};  // Le paso el path al hijo
    char * const envp[] = {NULL};
    execve("./md5Slave.out", argv, envp);

    perror("Error executing child program");
    exit(1);
}

//Uso argv[0] como write fd (para el pipe) y argv[1] como read fd (para el pipe)
//El read and write fd los necesito para cerrar los pipes a los md5Slave
int main() {

    pid_t child_pids[NUM_CHILD];
    int pipe_fds[NUM_CHILD][2];  // Array de fd's para el md5Slave


    int i = 0;
    char bufferWrite[128];  //Write to parent
    char bufferRead[128];   //Read from parent

    while(TRUE){

        if (pipe(pipe_fds[i]) == -1) {
            perror("Error creating pipe");
            exit(1);
        }


        ssize_t bytes_r = read(STDIN_FILENO,bufferRead,sizeof(bufferRead));
        bufferRead[bytes_r] = '\0';


        child_pids[i] = fork();

        if (child_pids[i] == -1) {
            perror("Error creating child process");
            exit(1);
        } else if (child_pids[i] == 0) {  // hijo
            child_process(pipe_fds[i][0], pipe_fds[i][1], bufferRead);
            exit(0);
        } else {  // padre
            close(pipe_fds[i][1]);

            ssize_t bytes_read = read(pipe_fds[i][0],bufferWrite,sizeof(bufferWrite));
            bufferWrite[bytes_read] = '\0';                                     //asegurar que el '\0' este bien
            printf("%d %s\n",getpid(),bufferWrite);

            close(pipe_fds[i][0]);
            i++;

            if(i == 20){  //Para ese entonces el proceso hijo 0 ya va a haber terminado, entonces puedo rehusar el fd
                i = 0;
            }
        }
    }

    exit(0);
}