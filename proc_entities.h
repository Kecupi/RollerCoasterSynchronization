/**
 *  @file proc_entities.h
 *  @brief Header of library used for handling of entities used inside process synchronization
 *
 *  Library header contains functions used for creating and managaing process entities
 *
 *  @author Stepan Horenek
 *  @date 6. 8. 2026
*/

#ifndef __IOS_PROC_ENT
#define __IOS_PROC_ENT

#include <stdio.h>
#include <stdlib.h>

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

/**
 *  @brief function for handling of dispatcher processes children
*/
void dispatcher();

#endif