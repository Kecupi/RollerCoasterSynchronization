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
#include "proc_entities.h"

/**
 *  @brief function for conversion of char* from input to int
 *  @param convertible char* to be converted to int
 *  @param value pointer to save conversion result in
 *  @return 0 if success, 1 if conversion fails
*/
int val_to_int(char* convertible, int* value);
/**
 *  @brief function to check whether input value is between boundaries, loads values into config struct
 *  @param argument pointer to value from input to check against boundaries
 *  @param min minimal value of type to check value against
 *  @param max maximal value of type to check value against
 *  @param type string part of error message to print in case of failure
 *  @param config_part pointer to int inside config structure to be populated in this function
 *  @return 0 if successful, 1 if problem in function or subfunctions
*/
int check_input_part(char* value, int min, int max, char* type, int* config_part);
/**
 *  @brief function for checking all arguments from input, passes them alongside config struct to other functions
 *  @param argc pointer to number of arguments, including program itself
 *  @param argv list of arguments
 *  @param config pointer of config structure to populate with data form user input
 *  @return 0 if successful, 1 if atleast 1 of checks fails
*/
int check_input(int *argc, char **argv, sync_config* config);

#endif