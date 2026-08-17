/**
 *  @file proc_main.c
 *  @brief Main file of the program working as a backbone of the project
 *
 *  The file takes input of the user in certain ranges and runs a simulation of an amusement park ride using processes
 
 *  @author Stepan Horenek
 *  @date 17. 8. 2026
*/
#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "proc_input.h"
#include "proc_entities.h"

typedef struct {
    int actions_cnt;
    int visitor_cnt;
    int cart_cnt;
} shared_struct;

int main (int argc, char** argv){
    if ((check_input(&argc, &argv)) == 1){ // check input parameters
        return 1;
    }
    FILE* output = fopen("log.out", "w");
    if (output == NULL){
        fprintf(stderr, "ERROR: Couldn't open log file for write\n");
        return 1;
    }
    // shared memory initialization
    const char* shared_name = "/shared_info";
    int shared = shm_open(shared_name, O_RDWR | O_CREAT, 0666);
    if (shared == -1){
        fprintf(stderr, "ERROR: Failed to allocate shared memory\n");
        fclose(output);
        return 1;
    }
    ftruncate(shared, sizeof(shared_struct));
    shared_struct* shared_mem = mmap(NULL, sizeof(shared_struct), PROT_READ | PROT_WRITE, MAP_SHARED, shared, 0);
    if (shared_mem == MAP_FAILED){
        fprintf(stderr, "Error: Failed to map shared memory\n");
        close(shared);
        fclose(output);
        return 1;
    }
    pid_t disp_pid = fork();
    if (disp_pid < 0){
        fprintf(stderr, "ERROR: Failed to create dispatcher process");
        munmap(shared_mem, sizeof(shared_struct));
        close(shared);
        fclose(output);
        return 1;
    } else if (disp_pid == 0){
        dispatcher();
    }
    munmap(shared_mem, sizeof(shared_struct));
    close(shared);
    shm_unlink(shared_name);
    fclose(output);
}