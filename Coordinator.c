#include "Checker.h"
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
    //fail to proceed without sufficient arguments
    if(argc-1 != 5){
        printf("Coordinator [%i]: Incorrect number of arguments (%i)\n", getpid(), argc-1);
        return 0;
    }
    //change type of pid
    pid_t pid[4];
    //create pipes with file descriptors, create shared memory
    int result;
    int fd[4][2];
    int shm_id[4], *shm_ptr[4];
    for(int i = 0; i < 4; i++){
        if(pipe(fd[i]) == -1){
            printf("Coordinator: pipe creation failed. Exiting.\n");
            exit(1);
        }
        shm_id[i] = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0666);
        //verify shmget success
        if(shm_id[i] < 0){
            printf("Coordinator: shmget() failed. Exiting.\n");
            exit(1);
        }
        shm_ptr[i] = (int*) shmat(shm_id[i], NULL, 0);
        //verify shmat success
        if((int)shm_ptr == -1){
            printf("Coordinator: shmat() failed. Exiting.\n");
            exit(1);
        }
    }

    //Assuming 5 arguments, this loop will iterate four times
    for(int i = 0; i < 4; i++){ 
        pid[i] = fork();
        //handle all possible values of fork
        //parent case
        if(pid[i] > 0){
            //announce current process ID 
            printf("Coordinator: forked process with ID %i.\n", pid[i]);
            //close read end of pipe, write to file descriptor
            close(fd[i][0]);
            write(fd[i][1], &shm_id[i], sizeof(int));
            close(fd[i][1]);
            printf("Coordinator: wrote shm ID %i to pipe (%lu bytes)\n", shm_id[i], sizeof(int));

        }
        //child case
        else if(pid[i] == 0){
            //send child to execute checker with command line arguments divisor and dividend
            close(fd[i][1]);
            char str[sizeof(fd[i][0])];
            sprintf(str, "%d", fd[i][0]);
            execlp("./Checker", "Coordinator", argv[1], argv[i+2], str, NULL);
            close(fd[i][0]);
        }
        //fail case
        else if(pid[i] == -1){
            //print error and exit
            printf("Coordinator: Fork failed\n");
            exit(1);
        }
    }
    //listen for results to be given back through pointer, report results
    for(int i = 0; i < 4; i++){
        int waitstatus_v;
        waitpid(pid[i], &waitstatus_v, 0);
        result = *shm_ptr[i];
        int divisor = atoi(argv[1]), dividend = atoi(argv[i+2]);
        if(result){
            printf("Coordinator: waiting on child process ID %i...\n", pid[i]);
            printf("Coordinator: result 1 read from shared memory: %i is divisible by %i.\n", dividend, divisor);
        }
        else{
            printf("Coordinator: waiting on child process ID %i...\n", pid[i]);
            printf("Coordinator: result 0 read from shared memory: %i is not divisible by %i.\n", dividend, divisor);
        }
        
    }
    //mark shared memory for deletion
    for(int i = 0; i < 4; i++){
        if(shmdt(shm_ptr[i])){
            printf("Coordinator: shmdt() failed. Exiting.\n");
            exit(1);
        }
        if(shmctl(shm_id[i], IPC_RMID, 0) < 0){
            printf("Coordinator: shmctl() failed. Exiting.\n");
            exit(1);
        }
    }
    printf("Coordinator: exiting.\n");
    return 0;
}

