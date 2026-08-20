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
    shared_mem->actions_cnt++;
    printf("%d: D started\n", shared_mem->actions_cnt);
    int to_serve = config->visitor_num;
    sem_post(&shared_mem->write_mutex);
    while(1){
        sem_wait(&shared_mem->write_mutex);
        if (to_serve <= 0){
            shared_mem->next_cart_cnt = 0;
            sem_post(&shared_mem->write_mutex);
            for (int cnt = 0; cnt < config->cart_num; cnt++){
                sem_post(&shared_mem->next_cart);
            }
            sem_wait(&shared_mem->write_mutex);
            shared_mem->actions_cnt++;
            printf("%d: D closing\n", shared_mem->actions_cnt);
            sem_post(&shared_mem->write_mutex);
            munmap(shared_mem, sizeof(shared_struct));
            exit(0);
        } else if (to_serve < config->cart_capacity){
            shared_mem->next_cart_cnt = to_serve;
            to_serve -= config->cart_capacity;
        } else if (to_serve >= config->cart_capacity){
            shared_mem->next_cart_cnt = config->cart_capacity;
            to_serve -= config->cart_capacity;
        }
        shared_mem->actions_cnt++;
        printf("%d: D next cart\n", shared_mem->actions_cnt);
        sem_post(&shared_mem->next_cart);
        sem_post(&shared_mem->write_mutex);
        sem_wait(&shared_mem->cart_disp);
    }
}

/**
 *  @brief function for cart logic simulation
*/
void cart(int cart_id, shared_struct* shared_mem, sync_config* config){
    shared_mem->actions_cnt++;
    printf("%d: C %d: started\n", shared_mem->actions_cnt, cart_id);
    int current_capacity = 0;
    sem_post(&shared_mem->write_mutex);
    while(1){
        sem_wait(&shared_mem->next_cart);
        sem_wait(&shared_mem->write_mutex);
        current_capacity = shared_mem->next_cart_cnt;
        shared_mem->actions_cnt++;
        if (current_capacity <= 0){
            printf("%d: C %d: closed\n", shared_mem->actions_cnt, cart_id);
            sem_post(&shared_mem->write_mutex);
            munmap(shared_mem, sizeof(shared_struct));
            exit(0);
        }
        printf("%d: C %d: boarding started\n", shared_mem->actions_cnt, cart_id);
        sem_post(&shared_mem->write_mutex);
        for (int cnt = 0; cnt < current_capacity; cnt++){
            sem_post(&shared_mem->turnstile_enter);
        }
        for (int cnt = 0; cnt < current_capacity; cnt++){
            sem_wait(&shared_mem->boarding);
        }
        sem_wait(&shared_mem->write_mutex);
        shared_mem->actions_cnt++;
        printf("%d: C %d: boarding complete\n", shared_mem->actions_cnt, cart_id);
        sem_post(&shared_mem->write_mutex);

        sem_post(&shared_mem->cart_disp);

        sem_wait(&shared_mem->write_mutex);
        shared_mem->actions_cnt++;
        printf("%d: C %d: leaving started\n", shared_mem->actions_cnt, cart_id);
        sem_post(&shared_mem->write_mutex);
        for (int cnt = 0; cnt < current_capacity; cnt++){
            sem_post(&shared_mem->turnstile_leave);
        }
        for (int cnt = 0; cnt < current_capacity; cnt++){
            sem_wait(&shared_mem->unboarding);
        }
        sem_wait(&shared_mem->write_mutex);
        shared_mem->actions_cnt++;
        printf("%d: C %d: leaving complete\n", shared_mem->actions_cnt, cart_id);
        sem_post(&shared_mem->write_mutex);
    }
}

/**
 *  @brief function for visitor logic simulation
*/
void visitor(int visitor_id, shared_struct* shared_mem, sync_config* config){
    shared_mem->actions_cnt++;
    printf("%d: V %d: started\n", shared_mem->actions_cnt, visitor_id);
    sem_post(&shared_mem->write_mutex);
    // list into queue
    sem_wait(&shared_mem->write_mutex);
    shared_mem->actions_cnt++;
    printf("%d: V %d: queue\n", shared_mem->actions_cnt, visitor_id);
    sem_post(&shared_mem->write_mutex);
    // wait to be able to board
    sem_wait(&shared_mem->turnstile_enter);
    sem_wait(&shared_mem->write_mutex);
    shared_mem->actions_cnt++;
    printf("%d: V %d: boarding\n", shared_mem->actions_cnt, visitor_id);
    sem_post(&shared_mem->write_mutex);
    sem_post(&shared_mem->boarding);
    // wait to leave the attraction
    sem_wait(&shared_mem->turnstile_leave);
    sem_wait(&shared_mem->write_mutex);
    shared_mem->actions_cnt++;
    printf("%d: V %d: leaving\n", shared_mem->actions_cnt, visitor_id);
    sem_post(&shared_mem->write_mutex);
    sem_post(&shared_mem->unboarding);
    // unmap shared memory for this process
    munmap(shared_mem, sizeof(shared_struct));
    exit(0);
}