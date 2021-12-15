#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <string.h>

int main(int argc, char* argv[]){
    printf("Checker process [%i]: Starting.\n", getpid());
    //convert arguments to ints, give them workable names 
    int divisor = atoi(argv[1]);
    int dividend = atoi(argv[2]);
    //retrive file descriptor, use to grab shm_id
    int fd = atoi(argv[3]);
    int shm_id;
    read(fd, &shm_id, sizeof(int));
    
    int* shm_ptr = (int*) shmat(shm_id, NULL, 0);
    
    //verify that smhat() was successful 
    if((int)shm_ptr == -1){
        printf("Checker: shmat() failed. Exiting.\n");
        exit(1);
    }
    printf("Checker process [%i]: read 4 bytes containing shm ID %i\n", getpid(), shm_id);
    //check divisibility
    if(dividend % divisor){
        //nonzero value of mod operation indicates a remainder; not divisible
        printf("Checker process [%i]: %i *IS NOT* divisible by %i.\n", getpid(), dividend, divisor);
        printf("Checker process [%i]: wrote result (0) to shared memory.\n", getpid());
        *shm_ptr = 0;
    }
    else{
        //zero value indicates no remainder; divisible
        printf("Checker process [%i]: %d *IS* divisible by %d.\n", getpid(), dividend, divisor);
        printf("Checker process [%i]: wrote result (1) to shared memory.\n", getpid());
        *shm_ptr = 1;
    }
    exit(0);
}
