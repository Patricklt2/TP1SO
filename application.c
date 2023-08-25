#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#define SLAVENUM 5
#define MAXFILESPERSLAVE 2

int main(int argc, char *argv[]) {

    int currentFile = 0;

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