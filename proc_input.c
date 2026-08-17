/**
 *  @file proc_input.c
 *  @brief Library used for parsing user input
 *
 *  Library receives argc and argv from user, converts each argv to int for comparison with macros from lirary header.
 *  If problem occurs, error message is printed and main program receives info to abort itself.
 *
 *  @author Stepan Horenek
 *  @date 1. 8. 2026
*/
#include <stdio.h>
#include <stdlib.h>
#include "proc_input.h"

/**
 *  @brief function for conversion of char* from input to int
*/
int val_to_int(char* convertible, int* value){
    char* end_ptr;
    *value = strtod(convertible, &end_ptr);
    if (*end_ptr == '\0'){
        return 0;
    } else if (convertible == end_ptr){
        fprintf(stderr, "ERROR: Invalid type of argument, can't be converted to int\n");
        return 1;
    } else {
        fprintf(stderr, "ERROR: Invalid character inside argument during conversion\n");
        return 1;
    }
    
}
/**
 *  @brief function to check whether input value is between boundaries, prints error message
*/
int check_input_part(char *argument, int min, int max, char *type, int* config_part){
    int value;
    if (val_to_int(argument, &value) == 1){
        return 1;
    }
    if ((value  < min) || (value > max)){
        fprintf(stderr, "ERROR: %s out of range (expected %d <= N <= %d, got %d)\n", type, min, max, value);
        return 1;
    }
    *config_part = value;
    return 0;
}
/**
 *  @brief function for checking all arguments from input
*/
int check_input(int *argc, char **argv, sync_config* config){
    if (*argc != ARG_NUM){
        fprintf(stderr, "ERROR: Invalid number of arguments (expected %d, got %d)\n", ARG_NUM, *argc);
        return 1;
    }
    if (check_input_part(argv[1], V_MIN, V_MAX, "Number of carts", &(config->cart_num))
        || check_input_part(argv[2], N_MIN, N_MAX, "Number of visitors", &(config->visitor_num))
        || check_input_part(argv[3], K_MIN, K_MAX, "Cart capacity", &(config->cart_capacity))
        || check_input_part(argv[4], TV_MIN, TV_MAX, "Cart time", &(config->cart_runtime))
        || check_input_part(argv[5], TN_MIN, TN_MAX, "Maximal visitor time", &(config->max_next_visitor_time))
        || check_input_part(argv[6], O_MIN, O_MAX, "Cart inteval", &(config->min_time_between_carts)))
    {
        return 1;
    }
    return 0;
}