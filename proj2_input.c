#include <stdio.h>
#include <stdlib.h>
#include "proj2_input.h"

/**
 *  @brief function for conversion of char* from input to int
*/
int val_to_int(char* convertible){
    return 0;
}
/**
 *  @brief function to check whether input value is between boundaries, prints error message
*/
int check_input_part(char *value, int min, int max, char *type){
    value = val_to_int(value);
    if ((*value  <= min) || (*value >= max)){
        fprintf(stderr, "ERROR: %s out of range (expected %d < V < %d, got %d)\n", type, min, max, *value);
        return 1;
    }
    return 0;
}
/**
 *  @brief function for checking all arguments from input
*/
int check_input(int *argc, char ***argv){
    if (*argc != ARG_NUM){
        fprintf(stderr, "ERROR: Invalid number of arguments (expected %d, got %d)\n", ARG_NUM, *argc);
        return 1;
    }
    if (check_input_part((*argv)[1], V_MIN, V_MAX, "Number of carts")
        || check_input_part((*argv)[2], N_MIN, N_MAX, "Number of visitors")
        || check_input_part((*argv)[3], K_MIN, K_MAX, "Cart capacity")
        || check_input_part((*argv)[4], TV_MIN, TV_MAX, "Cart time")
        || check_input_part((*argv)[5], TN_MIN, TN_MAX, "Maximal visitor time")
        || check_input_part((*argv)[6], O_MIN, O_MAX, "Cart inteval"))
    {
        return 1;
    }
    return 0;
}