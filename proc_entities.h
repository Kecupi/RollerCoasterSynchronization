/**
 *  @file proc_entities.h
 *  @brief Header of library used for handling of entities used inside process synchronization
 *
 *  Library header contains functions used for creating and managaing process entities
 *
 *  @author Stepan Horenek
 *  @date 18. 8. 2026
*/

#ifndef __IOS_PROC_ENT
#define __IOS_PROC_ENT

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

// valid number of arguments
#define ARG_NUM 7
// boundaries for number of carts
#define V_MIN 1
#define V_MAX 9
// boundaries for number of visitors
#define N_MIN 1
#define N_MAX 9999
// boundaries for cart capacity
#define K_MIN 4
#define K_MAX 40
// boundaries for cart runtime
#define TV_MIN 0
#define TV_MAX 1000
// boundaries for maximum next visitor time
#define TN_MIN 0
#define TN_MAX 1000
// boundaries for minimal interval between 2 carts
#define O_MIN 1
#define O_MAX 100

// structure for shared variables
typedef struct {
    // counters
    int actions_cnt;
    int visitor_cnt;
    int cart_cnt;
    // semaphores
    sem_t write_mutex;
    sem_t boarding;
    sem_t unboarding;
    sem_t turnstile;
} shared_struct;

// structure for configuration parameters
typedef struct{
    int cart_num;
    int visitor_num;
    int cart_capacity;
    int cart_runtime;
    int max_next_visitor_time;
    int min_time_between_carts;
} sync_config;

/**
 *  @brief function for handling of dispatcher processes children
 *  @param shared_mem structure containing semaphores and counters for processes
 *  @param config pointer of config structure used for configuring child processes
*/
void dispatcher(shared_struct* shared_mem, sync_config* config);

/**
 *  @brief function for cart logic simulation
 *  @param cart_id number of order in which carts were created (not PID)
 *  @param shared_mem structure containing semaphores and counters for processes
 *  @param config pointer of config structure used for configuring enviroment
*/
void cart(int cart_id, shared_struct* shared_mem, sync_config* config);

/**
 *  @brief function for visitor logic simulation
 *  @param visitor_id number of order in which visitors were created (not PID)
 *  @param shared_mem structure containing semaphores and counters for processes
 *  @param config pointer of config structure used for configuring enviroment
*/
void visitor(int visitor_id, shared_struct* shared_mem, sync_config* config);
#endif