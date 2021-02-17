/*
 * Library for the simulated disk: vdisk
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "disk.h"

// Bit map struct for tracking free and allocated blocks
typedef struct block_status {
    // 0 - Allocated
    // 1 - Free
    unsigned int alloc_status : 1;
} block_s;

// Bit map array for tracking block status
block_s block_st[NUM_BLOCKS];

void writeBlock(FILE *disk, int blockNum, char *buffer) {
    fseek(disk, blockNum * BLOCK_SIZE, 0);
    fwrite(buffer, BLOCK_SIZE, 1, disk);
}

void readBlock(FILE *disk, int blockNum, char *buffer) {
    printf("Reading block from disk.\n");
    fseek(disk, blockNum * BLOCK_SIZE, 0);
    fread(buffer, BLOCK_SIZE, 1, disk);
}

void InitBlock_st() {
    // Init bit map for block status
    printf("Initializing bitmap array to keep track of block status.\n");
    int i;

    for(i=0; i<10; i++) {
        block_st[i].alloc_status=0;
    }

    for(i=10; i<NUM_BLOCKS; i++) {
		block_st[i].alloc_status=1;
	}
}

void writeBlock_st(FILE *disk) {
	printf("Writing current block statuses based on data in memory.\n");
	
	int i, j = 1, c = 0;
	char byte = 0, byte_mask = 0;
	char buffer[BLOCK_SIZE];
	memset(&buffer, 0, BLOCK_SIZE);
	char *b = buffer;
	
	for(i=0; i<NUM_BLOCKS; i++) {
		switch(j) {
			case 1: byte_mask = 128;
				break;
			case 2: byte_mask = 64;
                break;
            case 3: byte_mask = 32;
                break;
            case 4: byte_mask = 16;
                break;
            case 5: byte_mask = 8;
                break;
            case 6: byte_mask = 4;
                break;
            case 7: byte_mask = 2;
                break;
            case 8: byte_mask = 1;
                break;
        }

        if(block_st[i].alloc_status == 1) {
            byte = byte | byte_mask;
        }

        j++;
        if(j == 9) {
            memcpy(b + (sizeof(char) * c), &byte, sizeof(char));
            c++;
            byte = 0;
            j = 1;
        }
    }
	
	writeBlock(disk, 1, buffer);
}

int findFreeBlock() {
    printf("Finding and returning value of free block.\n");
    int i;
    for(i=0; i<NUM_BLOCKS; i++) {
        if(block_st[i].alloc_status == 1) {
            return i;
        }
    }
    printf("Error: No free blocks available");
    exit(1);
}