#include <stdlib.h>
#include <stdio.h>

#include "../io/File.h"

int main() {
    printf("Test 2\n");

    InitLLFS();

    printf("Creating new file in root directory.\n");
    createFile("testfile", 0);
    printf("File, 'testfile' has been created.\n");

    return 0;
}