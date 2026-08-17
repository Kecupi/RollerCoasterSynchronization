/**
 *  @file proc_entities.c
 *  @brief Library used for handling of entities used inside process synchronization
 *
 *  Library contains functions used for creating and managaing process entities
 *
 *  @author Stepan Horenek
 *  @date 17. 8. 2026
*/

#include "proc_entities.h"

/**
 *  @brief function for handling of dispatcher processes children
*/
void dispatcher(shared_struct* shared_mem, char*** argv){
    sem_wait(&shared_mem->write_mutex);
    shared_mem->actions_cnt++;
    sem_post(&shared_mem->write_mutex);
    printf("%d: D: started\n", shared_mem->actions_cnt);
    exit(0);
    return;
}