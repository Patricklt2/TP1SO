#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#define SLAVENUM 5
#define MAXFILESPERSLAVE 20

typedef struct pipechannles{
    int pid;
    int slave_a_master[2];
    int master_a_slave[2];
}pipechannels;

int calculateSlavesNum(int fAmount);

int main(int argc, char *argv[]) {
    pipechannels pipes[SLAVENUM];
    int currentFile=0;

    int slavesAmount = calculateSlavesNum(argc - 1);

    for(int i=0;i<slavesAmount;i++){
        open(pipes[i].master_a_slave);//todo de ver como manipular los pipes
        open(pipes[i].slave_a_master);
        if((pipes[i].pid=fork())==0){
            close(pipes[i].master_a_slave[1]);
            close(pipes[i].slave_a_master[0]);
            dup2(pipes[i].slave_a_master[1],STDOUT_FILENO);
            dup2(pipes[i].master_a_slave[0],STDIN_FILENO);
            break;
         }
        else{
            close(pipes[i].master_a_slave[0]);
            close(pipes[i].slave_a_master[1]);
            dup2(pipes[i].slave_a_master[0],STDIN_FILENO);
            dup2(pipes[i].master_a_slave[1],STDOUT_FILENO);
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


int calculateSlavesNum(int fAmount) {
    return (fAmount < SLAVENUM) ? (fAmount) : (((fAmount / MAXFILESPERSLAVE) + 1) * SLAVENUM);
}