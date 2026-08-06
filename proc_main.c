/**
 *  @file proc_main.c
 *  @brief Main file of the program working as a backbone of the project
 *
 *  The file takes input of the user in certain ranges and runs a simulation of an amusement park ride using processes
 
 *  @author Stepan Horenek
 *  @date 6. 8. 2026
*/
#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "proc_input.h"

typedef struct {
    int actions;
} shared;

int main (int argc, char** argv){
    if ((check_input(&argc, &argv)) == 1){ // check input parameters
        return 1;
    }
    FILE* output = fopen("log.out", "w");
    if (output == NULL){
        fprintf(stderr, "ERROR: Couldn't open log file for write");
        return 1;
    }
    pid_t disp_pid = fork();
    if (disp_pid < 0){
        fprintf(stderr, "ERROR: Failed to create dispatcher process");
        return 1;
    } else {
        dispatcher();
    }
    fclose(output);
}