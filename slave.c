#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_CHILD 3

void child_process(int pipe_fd_read, int pipe_fd_write, char * fPath) {
    // Cierro pipe de lectura
    close(pipe_fd_read);

    // Redirijo la salida estandard del hijo
    dup2(pipe_fd_write, STDOUT_FILENO);

    // Proceso nuevo
    char * argv[] = {fPath,NULL};  // Le paso el path al hijo
    char * const envp[] = {NULL};
    execve("./md5Slave", argv, envp);

    perror("Error executing child program");
    exit(1);
}

int main() {
    pid_t child_pids[NUM_CHILD];
    int pipe_fds[NUM_CHILD][2];  // Array de fd's

    char *fPaths[NUM_CHILD] = {
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
    char buff[128];
    for (int i = 0; i < NUM_CHILD; i++) {
       ssize_t bytes_read = read(pipe_fds[i][0],buff,sizeof(buff));
       buff[bytes_read] = '\0';                                     //asegurar que el '\0' este bien
        printf("%d %s\n",getpid(),buff);
    }

    return 0;
}