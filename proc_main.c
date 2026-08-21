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
    FILE* log_file = fopen("log.out", "w"); // create log file
    if (log_file == NULL){
        fprintf(stderr, "ERROR: Couldn't open log file for write\n");
        free(config);
        exit(1);
    }
    freopen("log.out", "w", stdout);
    setbuf(stdout, NULL);
    fclose(log_file);
    // create shared memory
    const char* shared_name = "/shared_info";
    int shared_id = shm_open(shared_name, O_RDWR | O_CREAT, 0666);
    if (shared_id == -1){
        fprintf(stderr, "ERROR: Failed to allocate shared memory\n");
        free(config);
        exit(1);
    }
    // size shared memory
    if ((ftruncate(shared_id, sizeof(shared_struct))) == -1){
        fprintf(stderr, "Error: Failed to size shared memory\n");
        free(config);
        close(shared_id);
        shm_unlink(shared_name);
        exit(1);
    }
    // map shared memory
    shared_struct* shared_mem = mmap(NULL, sizeof(shared_struct), PROT_READ | PROT_WRITE, MAP_SHARED, shared_id, 0);
    if (shared_mem == MAP_FAILED){
        fprintf(stderr, "Error: Failed to map shared memory\n");
        free(config);
        close(shared_id);
        shm_unlink(shared_name);
        exit(1);
    }
    // initialize values in shared memory
    init_shared_values(shared_mem);
    pid_t* cart_pids = malloc(config->cart_num * sizeof(pid_t));
    if (cart_pids == NULL){
        fprintf(stderr, "ERROR: Failed to allocate memory for cart PIDs array\n");
        free(config);
        destroy_semaphores(shared_mem);
        destroy_shared_memory(shared_name, shared_id, shared_mem);
        exit(1);
    }
    pid_t* visitor_pids = malloc(config->visitor_num * sizeof(pid_t));
    if (visitor_pids == NULL){
        fprintf(stderr, "ERROR: Failed to allocate memory for visitor PIDs array\n");
        free(config);
        free(cart_pids);
        destroy_semaphores(shared_mem);
        destroy_shared_memory(shared_name, shared_id, shared_mem);
        exit(1);
    }
    // create dispatcher process
    sem_wait(&shared_mem->write_mutex);
    pid_t disp_pid = fork();
    if (disp_pid < 0){ // if fork failed
        fprintf(stderr, "ERROR: Failed to create dispatcher process\n");
        free(config);
        free(cart_pids);
        free(visitor_pids);
        destroy_semaphores(shared_mem);
        destroy_shared_memory(shared_name, shared_id, shared_mem);
        exit(1);
    } else if (disp_pid == 0){
        dispatcher(shared_mem, config);
    }
    // create cart processes
    for (int idx = 0; idx < config->cart_num; idx++){
        sem_wait(&shared_mem->write_mutex);
        pid_t cart_pid = fork();
        if (cart_pid < 0){ // if fork failed
            fprintf(stderr, "ERROR: Failed to create cart process\n");
            int carts_to_abort = shared_mem->cart_cnt;
            for (int cnt = 0; cnt < (config->visitor_num / config->cart_capacity + 1); cnt++){ // eliminate inner counter inside dispatcher
                sem_post(&shared_mem->cart_disp);
            }
            sem_post(&shared_mem->write_mutex);
            waitpid(disp_pid, NULL, 0); // wait for dispatcher to abort
            sem_wait(&shared_mem->write_mutex);
            for (int cnt = 0; cnt < shared_mem->cart_cnt * config->cart_capacity; cnt++){ // sends enough signals to get all carts to the end
                sem_post(&shared_mem->boarding);
                sem_post(&shared_mem->unboarding);
            }
            sem_post(&shared_mem->write_mutex);
            for (int idx = 0; idx < carts_to_abort; idx++){ // wait for all carts to abort
                waitpid(cart_pids[idx], NULL, 0);
            }
            free(config);
            free(cart_pids);
            free(visitor_pids);
            destroy_semaphores(shared_mem);
            destroy_shared_memory(shared_name, shared_id, shared_mem);
            exit(1);
        } else if (cart_pid == 0){
            cart_pids[shared_mem->cart_cnt] = cart_pid;
            shared_mem->cart_cnt++;
            cart(shared_mem->cart_cnt, shared_mem, config); // shared_mem->write_mutex is passed to cart
        }
    }
    for (int idx = 0; idx < config->visitor_num; idx++){
        sem_wait(&shared_mem->write_mutex);
        pid_t visitor_pid = fork();
        if (visitor_pid < 0){ // if fork failed
            fprintf(stderr, "ERROR: Failed to create visitor process\n");
            int carts_to_abort = shared_mem->cart_cnt;
            int visitors_to_abort = shared_mem->visitor_cnt;
            for (int cnt = 0; cnt < visitors_to_abort; cnt++){
                sem_post(&shared_mem->turnstile_enter);
                sem_post(&shared_mem->turnstile_leave);
            }
            sem_post(&shared_mem->write_mutex);
            for (int idx = 0; idx < visitors_to_abort; idx++){ // wait for all visitors to abort
                waitpid(visitor_pids[idx], NULL, 0);
            }
            sem_wait(&shared_mem->write_mutex);
            for (int cnt = 0; cnt < (config->visitor_num / config->cart_capacity + 1); cnt++){ // eliminate inner counter inside dispatcher
                sem_post(&shared_mem->cart_disp);
            }
            sem_post(&shared_mem->write_mutex);
            waitpid(disp_pid, NULL, 0); // wait for dispatcher to abort
            sem_wait(&shared_mem->write_mutex);
            for (int cnt = 0; cnt < shared_mem->cart_cnt * config->cart_capacity; cnt++){ // sends enough signals to get all carts to the end
                sem_post(&shared_mem->boarding);
                sem_post(&shared_mem->unboarding);
            }
            sem_post(&shared_mem->write_mutex);
            for (int idx = 0; idx < carts_to_abort; idx++){ // wait for all carts to abort
                waitpid(cart_pids[idx], NULL, 0);
            }
            free(config);
            free(cart_pids);
            free(visitor_pids);
            destroy_semaphores(shared_mem);
            destroy_shared_memory(shared_name, shared_id, shared_mem);
            exit(1);
        } else if (visitor_pid == 0){
            cart_pids[shared_mem->visitor_cnt] = visitor_pid;
            shared_mem->visitor_cnt++;
            visitor(shared_mem->visitor_cnt, shared_mem, config); // shared_mem->write_mutex is passed to visitor
        }
    }
    for (int idx = 0; idx < config->visitor_num; idx++){ // wait for all visitors to abort
        waitpid(visitor_pids[idx], NULL, 0);
    }
    for (int idx = 0; idx < config->cart_num; idx++){ // wait for all carts to abort
        waitpid(cart_pids[idx], NULL, 0);
    }
    waitpid(disp_pid, NULL, 0); // wait for dispatcher to end
    free(cart_pids);
    free(visitor_pids);
    free(config);
    destroy_semaphores(shared_mem);
    destroy_shared_memory(shared_name, shared_id, shared_mem);
    
}