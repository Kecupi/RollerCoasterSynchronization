/**
 *  @file proc_entities.c
 *  @brief Library used for handling of entities used inside process synchronization
 *
 *  Library contains functions used for creating and managaing process entities
 *
 *  @author Stepan Horenek
 *  @date 19. 8. 2026
*/

#include "proc_entities.h"

/**
 *  @brief function for handling of dispatcher processes children
*/
void dispatcher(shared_struct* shared_mem, sync_config* config){
    pid_t* cart_pids = malloc(config->cart_num*sizeof(int));
    if (cart_pids == NULL){
        fprintf(stderr, "ERROR: Failed to create list of cart process PIDs\n");
        munmap(shared_mem, sizeof(shared_struct));
        exit(0);
        return;
    }
    int* visitor_pids = malloc(config->visitor_num*sizeof(int));
    if (cart_pids == NULL){
        fprintf(stderr, "ERROR: Failed to create list of visitor process PIDs\n");
        munmap(shared_mem, sizeof(shared_struct));
        free(cart_pids);
        exit(0);
        return;
    }
    sem_wait(&shared_mem->write_mutex);
    shared_mem->actions_cnt++;
    printf("%d: D: started\n", shared_mem->actions_cnt);
    sem_post(&shared_mem->write_mutex);
    for (int idx = 0; idx < config->visitor_num; idx++){
        sem_wait(&shared_mem->write_mutex);
        shared_mem->visitor_cnt++;
        pid_t visitor_pid = fork();
        if (visitor_pid < 0){
            fprintf(stderr, "ERROR: Failed to create visitor process\n");
            // TODO: signal already existing processes to abort
            munmap(shared_mem, sizeof(shared_struct));
            free(cart_pids);
            free(visitor_pids);
            exit(1);
            return;
        } else if (visitor_pid == 0){
            visitor(shared_mem->visitor_cnt, shared_mem, config);
        } else {
            visitor_pids[shared_mem->visitor_cnt - 1] = visitor_pid;
        }
    }
    for (int idx = 0; idx < config->cart_num; idx++){
        sem_wait(&shared_mem->write_mutex);
        shared_mem->cart_cnt++;
        pid_t cart_pid = fork();
        if (cart_pid < 0){
            fprintf(stderr, "ERROR: Failed to create cart process\n");
            // TODO: signal already existing processes to abort
            munmap(shared_mem, sizeof(shared_struct));
            free(cart_pids);
            free(visitor_pids);
            exit(1);
            return;
        } else if (cart_pid == 0){
            cart(shared_mem->cart_cnt, shared_mem, config);
        } else {
            cart_pids[shared_mem->cart_cnt - 1] = cart_pid;
        }
    }
    for (int idx = 0; idx < config->cart_num; idx++){
        waitpid(cart_pids[idx], NULL, 0);
    }
    for (int idx = 0; idx < config->visitor_num; idx++){
        waitpid(visitor_pids[idx], NULL, 0);
    }
    free(cart_pids);
    free(visitor_pids);
    munmap(shared_mem, sizeof(shared_struct));
    exit(0);
    return;
}

/**
 *  @brief function for cart logic simulation
*/
void cart(int cart_id, shared_struct* shared_mem, sync_config* config){
    shared_mem->actions_cnt++;
    printf("%d: C: %d: started\n", shared_mem->actions_cnt, cart_id);
    sem_post(&shared_mem->write_mutex);
    munmap(shared_mem, sizeof(shared_struct));
    while(1){
        sem_wait(&shared_mem->write_mutex);
        shared_mem->actions_cnt++;
        printf("%d: C: %d: boarding started\n", shared_mem->actions_cnt, cart_id);
        sem_post(&shared_mem->write_mutex);
        for (int cnt = 0; cnt < config->cart_capacity;cnt++){
            sem_post(&shared_mem->turnstile_enter);
        }
        for (int cnt = 0; cnt < config->cart_capacity;cnt++){
            sem_wait(&shared_mem->boarding);
        }
        sem_wait(&shared_mem->write_mutex);
        shared_mem->actions_cnt++;
        printf("%d: C: %d: boarding complete\n", shared_mem->actions_cnt, cart_id);
        sem_post(&shared_mem->write_mutex);

        sem_wait(&shared_mem->write_mutex);
        shared_mem->actions_cnt++;
        printf("%d: C: %d: leaving started\n", shared_mem->actions_cnt, cart_id);
        sem_post(&shared_mem->write_mutex);
        for (int cnt = 0; cnt < config->cart_capacity;cnt++){
            sem_post(&shared_mem->turnstile_leave);
        }
        for (int cnt = 0; cnt < config->cart_capacity;cnt++){
            sem_wait(&shared_mem->unboarding);
        }
        sem_wait(&shared_mem->write_mutex);
        shared_mem->actions_cnt++;
        printf("%d: C: %d: leaving complete\n", shared_mem->actions_cnt, cart_id);
        sem_post(&shared_mem->write_mutex);
    }
    sem_wait(&shared_mem->write_mutex);
    shared_mem->actions_cnt++;
    printf("%d: C: %d: closed\n", shared_mem->actions_cnt, cart_id);
    sem_post(&shared_mem->write_mutex);
    exit(0);
}

/**
 *  @brief function for visitor logic simulation
*/
void visitor(int visitor_id, shared_struct* shared_mem, sync_config* config){
    shared_mem->actions_cnt++;
    printf("%d: V: %d: started\n", shared_mem->actions_cnt, visitor_id);
    sem_post(&shared_mem->write_mutex);
    // list into queue
    sem_wait(&shared_mem->write_mutex);
    shared_mem->actions_cnt++;
    printf("%d: V: %d: queue\n", shared_mem->actions_cnt, visitor_id);
    sem_post(&shared_mem->write_mutex);
    // wait to be able to board
    sem_wait(&shared_mem->turnstile_enter);
    sem_wait(&shared_mem->write_mutex);
    shared_mem->actions_cnt++;
    printf("%d: V: %d: boarding\n", shared_mem->actions_cnt, visitor_id);
    sem_post(&shared_mem->write_mutex);
    sem_post(&shared_mem->boarding);
    // wait to leave the attraction
    sem_wait(&shared_mem->turnstile_leave);
    sem_wait(&shared_mem->write_mutex);
    shared_mem->actions_cnt++;
    printf("%d: V: %d: leaving\n", shared_mem->actions_cnt, visitor_id);
    sem_post(&shared_mem->write_mutex);
    sem_post(&shared_mem->unboarding);
    // unmap shared memory for this process
    munmap(shared_mem, sizeof(shared_struct));
    exit(0);
}