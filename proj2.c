#include <stdio.h>
#include "proj2_input.h"

int main (int argc, char** argv){
    if ((check_input(&argc, &argv)) == 1){ // check input parameters
        return 1;
    }
}