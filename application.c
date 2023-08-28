#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define SLAVENUM 5
#define MAXFILESPERSLAVE 20

int main(int argc, char *files[]) {

    int fAmount = argc - 1;
    int currentFile = 0;

    //creates a buffer and prints the information necessary in stdout for the vista process to connect

    //waits 2 seconds for a vista process

    //decides how many slave processes need to be created and initializes them

    pid_t* slaveProcesses = createSlaveProcesses(fAmount);

    //waits until a slave process finishes, and writes the result on the buffer

    //assigns a new file to the slave process, if there's none left, the slave is killed
    if(currentFile > MAXFILESPERSLAVE) {
        //kills
    } else {
        //gives the slave a new file
    }

    //once all the slaves finish, writes to the result file and returns
}


pid_t* createSlaveProcesses(int fAmount) {
    //creates as many slaves as files if the number is too low, creates x slaves per y files (eg. 5 slaves every 20 files)
    int slaves = (fAmount < SLAVENUM) ? (fAmount) : ((fAmount / MAXFILESPERSLAVE) * SLAVENUM);

    pid_t* slaveArr = malloc(slaves * sizeof(pid_t));

    for(int i=0; i<slaves; i++) {
        //creates slaves and stores them in the array
    }

    return slaveArr;
}