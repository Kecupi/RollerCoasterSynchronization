/**
 *  @file proc_main.c
 *  @brief Main file of the program working as a backbone of the project
 *
 *  The program takes input of the user in certain ranges and runs a simulation of an amusement park ride using processes
 
 *  @author Stepan Horenek
 *  @date 20. 8. 2026
*/
#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "proc_input.h"
#include "proc_entities.h"

/**
 *  @brief Function for initialization of values in shared memory
 *  @param shared_mem pointer to shared structure to be initialized
*/
void init_shared_values(shared_struct* shared_mem){
    shared_mem->actions_cnt = 0;
    shared_mem->visitor_cnt = 0;
    shared_mem->cart_cnt = 0;
    shared_mem->next_cart_cnt = 0;
    sem_init(&shared_mem->write_mutex, 1, 1);
    sem_init(&shared_mem->next_cart, 1, 0);
    sem_init(&shared_mem->cart_disp, 1, 0);
    sem_init(&shared_mem->turnstile_enter, 1, 0);
    sem_init(&shared_mem->turnstile_leave, 1, 0);
    sem_init(&shared_mem->boarding, 1, 0);
    sem_init(&shared_mem->unboarding, 1, 0);
}

/**
 *  @brief Function for destruction of semaphore files
 *  @param shared_mem pointer to shared structure to be initialized
*/
void destroy_semaphores(shared_struct* shared_mem){
    sem_destroy(&shared_mem->write_mutex);
    sem_destroy(&shared_mem->next_cart);
    sem_destroy(&shared_mem->cart_disp);
    sem_destroy(&shared_mem->turnstile_enter);
    sem_destroy(&shared_mem->turnstile_leave);
    sem_destroy(&shared_mem->boarding);
    sem_destroy(&shared_mem->unboarding);
}

/**
 *  @brief Function for destruction of shared memory
 *  @param shared_name text name of shared memory segment
 *  @param shared_id id of descriptor returned by shm_open()
 *  @param shared_mem pointer to shared structure to be initialized
*/
void destroy_shared_memory(const char* shared_name, int shared_id, shared_struct* shared_mem){
    munmap(shared_mem, sizeof(shared_struct));
    close(shared_id);
    shm_unlink(shared_name);
}

int main (int argc, char** argv){
    sync_config* config = malloc(sizeof(sync_config)); // create structure for input settings
    if (config == NULL){
        fprintf(stderr, "Error: Couldn't allocate data for simulation config\n");
        exit(1);
    }
    if ((check_input(&argc, argv, config)) == 1){ // check input parameters
        free(config);
        exit(1);
    }
    FILE* output = fopen("log.out", "w"); // create log file
    if (output == NULL){
        fprintf(stderr, "ERROR: Couldn't open log file for write\n");
        free(config);
        exit(1);
    }
    // create shared memory
    const char* shared_name = "/shared_info";
    int shared_id = shm_open(shared_name, O_RDWR | O_CREAT, 0666);
    if (shared_id == -1){
        fprintf(stderr, "ERROR: Failed to allocate shared memory\n");
        free(config);
        fclose(output);
        exit(1);
    }
    // size shared memory
    if ((ftruncate(shared_id, sizeof(shared_struct))) == -1){
        fprintf(stderr, "Error: Failed to size shared memory\n");
        free(config);
        close(shared_id);
        shm_unlink(shared_name);
        fclose(output);
        exit(1);
    }
    // map shared memory
    shared_struct* shared_mem = mmap(NULL, sizeof(shared_struct), PROT_READ | PROT_WRITE, MAP_SHARED, shared_id, 0);
    if (shared_mem == MAP_FAILED){
        fprintf(stderr, "Error: Failed to map shared memory\n");
        free(config);
        close(shared_id);
        shm_unlink(shared_name);
        fclose(output);
        exit(1);
    }
    // initialize values in shared memory
    init_shared_values(shared_mem);
    // create dispatcher process
    sem_wait(&shared_mem->write_mutex);
    pid_t disp_pid = fork();
    if (disp_pid < 0){ // if fork failed
        fprintf(stderr, "ERROR: Failed to create dispatcher process\n");
        free(config);
        destroy_semaphores(shared_mem);
        destroy_shared_memory(shared_name, shared_id, shared_mem);
        fclose(output);
        exit(1);
    } else if (disp_pid == 0){
        dispatcher(shared_mem, config);
    }
    // create cart processes
    for (int idx = 0; idx < config->cart_num; idx++){
        sem_wait(&shared_mem->write_mutex);
        shared_mem->cart_cnt++;
        pid_t cart_pid = fork();
        if (cart_pid < 0){ // if fork failed
            fprintf(stderr, "ERROR: Failed to create cart process\n");
            // TODO: signal already existing processes to abort
            free(config);
            destroy_semaphores(shared_mem);
            destroy_shared_memory(shared_name, shared_id, shared_mem);
            fclose(output);
            exit(1);
        } else if (cart_pid == 0){
            cart(shared_mem->cart_cnt, shared_mem, config); // shared_mem->write_mutex is passed to cart
        }
    }
    for (int idx = 0; idx < config->visitor_num; idx++){
        sem_wait(&shared_mem->write_mutex);
        shared_mem->visitor_cnt++;
        pid_t visitor_pid = fork();
        if (visitor_pid < 0){ // if fork failed
            fprintf(stderr, "ERROR: Failed to create visitor process\n");
            // TODO: signal already existing processes to abort
            free(config);
            destroy_semaphores(shared_mem);
            destroy_shared_memory(shared_name, shared_id, shared_mem);
            fclose(output);
            exit(1);
        } else if (visitor_pid == 0){
            visitor(shared_mem->visitor_cnt, shared_mem, config); // shared_mem->write_mutex is passed to visitor
        }
    }
    waitpid(disp_pid, NULL, 0); // wait for dispatcher to end
    free(config);
    destroy_semaphores(shared_mem);
    destroy_shared_memory(shared_name, shared_id, shared_mem);
    fclose(output);
}