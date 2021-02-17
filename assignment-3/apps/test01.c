#include <stdlib.h>
#include <stdio.h>

#include "../io/File.h"

int main() {
    printf("Test 1:\nCreating and manipulating the simulated disk, and formatting that disk.\n\n");

    printf("Creating the simulated disk and testing writing of blocks:\n");
    InitLLFS();

    printf("Testing reading of blocks.\n");
    testRead();

    return 0;
}