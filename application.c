#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#define SLAVENUM 5
#define MAXFILESPERSLAVE 2

typedef struct pipechannles{
    int pid;
    int pipe_de_slave[2];
    int pipe_a_slave[2];
}pipechannels;

int main(int argc, char *argv[]) {
    pipechannels pipes[SLAVENUM];
    for(int i=0;i<SLAVENUM){
        open(pipes[i].pipe_de_slave);//todo de ver como manipular los pipes
        open(pipe[i].pipe_a_slave);
        if((pipes[i].pid=fork())==0){
            break; //unica form
        }
    }
    //creates a buffer and prints the information necessary in stdout for the vista process to connect

    //waits 2 seconds for a vista process

    //decides how many slave processes need to be created and initializes them

    //waits until a slave process finishes, and writes the result on the buffer

    //assigns a new file to the slave process, if there's none left, the slave is killed
    if(currentFile > MAXFILESPERSLAVE) {
        //kills
    } else {
        //gives the slave a new file
    }

    //once all the slaves finish, writes to the result file and returns
}
