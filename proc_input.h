/**
 *  @file proc_input.h
 *  @brief Header of library used for parsing user input
 *
 *  Library header contains macros for limits of specific parameters that are supposed to be used to run program  
 *
 *  @author Stepan Horenek
 *  @date 1. 8. 2026
*/
#ifndef __IOS_PROC_IN
#define __IOS_PROC_IN

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
 *  @brief function for conversion of char* from input to int
 *  @param convertible char* to be converted to int
 *  @param value pointer to save conversion result in
 *  @return 0 if success, 1 if conversion fails
*/
int val_to_int(char* convertible, int* value);
/**
 *  @brief function to check whether input value is between boundaries, prints error message
 *  @param argument pointer to value from input to check against boundaries
 *  @param min minimal value of type to check value against
 *  @param max maximal value of type to check value against
 *  @param type string part of error message to print in case of failure
 *  @return 0 if successful, 1 if problem in function or subfunctions
*/
int check_input_part(char* value, int min, int max, char* type);
/**
 *  @brief function for checking all arguments from input
 *  @param argc pointer to number of arguments, including program itself
 *  @param argv pointer to list of arguments
 *  @return 0 if successful, 1 if atleast 1 of checks fails
*/
int check_input(int *argc, char ***argv);

#endif