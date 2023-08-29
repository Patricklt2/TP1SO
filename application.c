#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define SLAVENUM 5
#define MAXFILESPERSLAVE 20
#define SLAVE_PATH "b"

pid_t* createSlaveProcesses(int fAmount);

int main(int argc, char *files[]) {

    int fAmount = argc - 1;
    int currentFile = 0;
    printf("%d\n", argc);

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
    int slaves = (fAmount < SLAVENUM) ? (fAmount) : (((fAmount / MAXFILESPERSLAVE) + 1) * SLAVENUM);


    pid_t* slaveArr = malloc(slaves * sizeof(pid_t));
    for(int i=0; i<slaves; i++) {
        //creates slaves and stores them in the array
        pid_t child_pid = fork();
        if (child_pid == 0) {
            printf("creating slave process\n");
            //creates the slave process here
            exit(1);
        } else if (child_pid > 0) {
            slaveArr[i] = child_pid;
        } else {
            perror("error in fork creation");
            exit(1);
        }
    }

    return slaveArr;
}