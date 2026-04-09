#include <stdio.h>

#define ARG_NUM 7

int check_input(int *argc, char ***argv){
    if (*argc != ARG_NUM){
        fprintf(stderr, "ERROR: Invalid numbers of arguments (expected %d, got %d)", ARG_NUM, *argc);
        return 1;
    }
}

int main (int argc, char** argv){
    if ((check_input()) == 1){
        return 1;
    }
}