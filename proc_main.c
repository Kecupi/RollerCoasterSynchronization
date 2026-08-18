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

int main (int argc, char** argv){
    sync_config* config = malloc(sizeof(sync_config));
    if (config == NULL){
        fprintf(stderr, "Error: Couldn't allocate data for simulation config\n");
        return 1;
    }
    if ((check_input(&argc, argv, config)) == 1){ // check input parameters
        free(config);
        return 1;
    }
    FILE* output = fopen("log.out", "w");
    if (output == NULL){
        fprintf(stderr, "ERROR: Couldn't open log file for write\n");
        free(config);
        return 1;
    }
    // shared memory initialization
    const char* shared_name = "/shared_info";
    int shared = shm_open(shared_name, O_RDWR | O_CREAT, 0666);
    if (shared == -1){
        fprintf(stderr, "ERROR: Failed to allocate shared memory\n");
        free(config);
        fclose(output);
        return 1;
    }
    ftruncate(shared, sizeof(shared_struct));
    shared_struct* shared_mem = mmap(NULL, sizeof(shared_struct), PROT_READ | PROT_WRITE, MAP_SHARED, shared, 0);
    if (shared_mem == MAP_FAILED){
        fprintf(stderr, "Error: Failed to map shared memory\n");
        free(config);
        close(shared);
        fclose(output);
        return 1;
    }
    // init value of shared memory
    shared_mem->actions_cnt = 0;
    shared_mem->visitor_cnt = 0;
    shared_mem->cart_cnt = 0;
    sem_init(&shared_mem->write_mutex, 1, 1);
    sem_init(&shared_mem->turnstile, 1, 1);
    sem_init(&shared_mem->boarding, 1, 0);
    sem_init(&shared_mem->unboarding, 1, 0);
    // create dispatcher process
    pid_t disp_pid = fork();
    if (disp_pid < 0){
        fprintf(stderr, "ERROR: Failed to create dispatcher process");
        sem_destroy(&shared_mem->write_mutex);
        sem_destroy(&shared_mem->turnstile);
        sem_destroy(&shared_mem->boarding);
        sem_destroy(&shared_mem->unboarding);
        free(config);
        munmap(shared_mem, sizeof(shared_struct));
        close(shared);
        fclose(output);
        return 1;
    } else if (disp_pid == 0){
        dispatcher(shared_mem, config);
    }
    free(config);
    sem_destroy(&shared_mem->write_mutex);
    sem_destroy(&shared_mem->turnstile);
    sem_destroy(&shared_mem->boarding);
    sem_destroy(&shared_mem->unboarding);
    munmap(shared_mem, sizeof(shared_struct));
    close(shared);
    shm_unlink(shared_name);
    fclose(output);
}