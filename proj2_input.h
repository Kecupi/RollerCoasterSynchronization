#ifndef __IOS_PROJ2_IN
#define __IOS_PROJ2_IN

#include <stdio.h>
#include <stdlib.h>

// valid number of arguments
#define ARG_NUM 7
// boundaries for number of carts
#define V_MIN 0
#define V_MAX 10
// boundaries for number of visitors
#define N_MIN 0
#define N_MAX 10000
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
#define O_MIN 0
#define O_MAX 100

/**
 *  @brief function for conversion of char* from input to int
 *  @return result of conversion if successful, -1 otherwise
*/
int val_to_int(char* convertible);
/**
 *  @brief function to check whether input value is between boundaries, prints error message
 *  @param value pointer to value from input to check against boundaries
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