/**
 *  @file proj2.c
 *  @brief Main file of the program working as a backbone of the project
 *
 *  The file takes input of the user in certain ranges and runs a simulation of an amusement park ride using processes
 
 *  @author Stepan Horenek
 *  @date 1. 8. 2026
*/
#include <stdio.h>
#include "proj2_input.h"

int main (int argc, char** argv){
    if ((check_input(&argc, &argv)) == 1){ // check input parameters
        return 1;
    }
}