/*
 * Basis for filesystem
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "File.h"
#include "../disk/disk.h"

#define NUM_INODES 64

#define FLAT_FILE 0
#define DIR_FILE 1

// inode struct
typedef struct inode {
    int size; // 4 bytes
    int flag; // 4 bytes
    short int blocknums[10]; // 20 bytes
    short int single_indirect_block; // 2 bytes
    short int double_indirect_block; // 2 bytes
} inode;

inode inode_list[NUM_INODES];
int used_inodes = 0;

void InitLLFS() {
    printf("Initializing LLFS.\n");

    // Create or open vdisk
    FILE *disk = fopen("/home/tarushroy/CSC360/a3/disk/vdisk", "wb+");

    // Set bytes of vdisk to 0
    char init_buffer[BLOCK_SIZE * NUM_BLOCKS];
    memset(&init_buffer, 0, BLOCK_SIZE * NUM_BLOCKS);
    fseek(disk, 0, 0);
    fwrite(&init_buffer, 1, BLOCK_SIZE * NUM_BLOCKS, disk);
	
	// Init Block 0 - Superblock
	char buffer[BLOCK_SIZE];
	char *b = buffer;
	char magic[4] = "LLFS";
    int nblocks = NUM_BLOCKS, ninodes = NUM_INODES;
    memcpy(b, &magic, sizeof(magic));
    memcpy(b + sizeof(magic), &nblocks, sizeof(int));
    memcpy(b + sizeof(magic) + sizeof(int), &ninodes, sizeof(int));
    writeBlock(disk, 0, buffer);
	
	// Init Block 1 - Free block vector
	// Keeping track of free & used blocks
	// Init bit map
	printf("Initializing block 1 bits.\n");
	InitBlock_st();
	writeBlock_st(disk);
	
	// Blocks 2 - 5 : INODE AREA
    int i, j, k;
    // Init inodes
    for(i=0; i<NUM_INODES; i++) {
        inode_list[i].size = 0;
        inode_list[i].flag = 0;
        for(j=0; j<10; j++) {
            inode_list[i].blocknums[j]=0;
        }
    }
	
	// Create Root Directory
    createFile("/", DIR_FILE);
	
	 // Write inodes to vdisk
    printf("Initializing and writing initial inode values.\n");
    char cur_inode[32];
    memset(&buffer, 0, sizeof(BLOCK_SIZE));
    for(k=0; k<4; k++) {
        for(i=k*16; i<16; i++) {
            memcpy(&cur_inode, &inode_list[i].size, sizeof(int));
            memcpy(&cur_inode + sizeof(int), &inode_list[i].flag, sizeof(int));
            for(j=0; j<10; j++) {
                memcpy(&cur_inode + sizeof(int) + (sizeof(short int) * j), &inode_list[i].blocknums[j], sizeof(short int));
            }
            memcpy(&cur_inode + sizeof(int) + sizeof(short int)*10, &inode_list[i].single_indirect_block, sizeof(short int));
            memcpy(&cur_inode + sizeof(int) + sizeof(short int)*10 + sizeof(short int), &inode_list[i].double_indirect_block, sizeof(short int));
            memcpy(&buffer + (sizeof(inode) * i), &cur_inode, sizeof(inode));
        }
        writeBlock(disk, 2+k, buffer);
    }

    fclose(disk);
}

void createFile(char *filename, int flag) {
    if(sizeof(filename) > 30) {
        printf("Filename is longer than 30 bytes.");
        exit(1);
    }

    char fname[31] = "";
    strcpy(fname, filename);

    FILE *disk = fopen("/home/tarushroy/CSC360/a3/disk/vdisk", "rb+");

    int blockNum, inodeNum;
    unsigned char inn;
    char buffer[BLOCK_SIZE];
    char *b = buffer;
    memset(buffer, 0, BLOCK_SIZE);

    blockNum = findFreeBlock();
    inodeNum = createINODE(blockNum, flag);
    inn = inodeNum;

    if(flag == DIR_FILE) {
        memcpy(buffer, &inn, sizeof(inn));
        memcpy(buffer + sizeof(inn), fname, sizeof(fname));

        writeBlock(disk, blockNum, buffer);
    }

    if(flag == FLAT_FILE) {
        num_files_root++;

        memset(buffer, 0, BLOCK_SIZE);
        readBlock(disk, rootblocknum, buffer);

        memcpy(b + (32*num_files_root), &inn, sizeof(inn));
        memcpy(b + (32*num_files_root) + sizeof(inn), fname, sizeof(fname));

        writeBlock(disk, rootblocknum, buffer);
    }

    fclose(disk);
}

void deleteFile(char *filename) {

}

void writeToFile(char *pathname, char *buffer) {
	FILE *disk = fopen("/home/tarushroy/CSC360/a3/disk/vdisk", "rb+");

    int blocknum;
    char buff[BLOCK_SIZE];
    char *b = buff;
    readBlock(disk, rootblocknum, buff);
    char entry[32];
    memcpy(entry, b + sizeof(entry), sizeof(entry));
    char inn;
    memcpy(&inn, b, sizeof(inn));
    int iinn = atoi(&inn);
    char fname[31];
    memcpy(fname, entry + 1, sizeof(fname));

    if(fname == pathname) {
        blocknum = inode_list[iinn].blocknums[0];

        memset(buff, 0, BLOCK_SIZE);
        memcpy(buff, buffer, sizeof(*buffer));

        writeBlock(disk, blocknum, buff);
    }

    fclose(disk);
}

void readFile(char *pathname, char *buffer) {

}

void fsck() {

}

int createINODE(int blockNum, int flag) {
    if(used_inodes == 64) {
        printf("Can't create new inode. All inodes used.\n");
        exit(1);
    }

    inode_list[used_inodes].blocknums[0] = blockNum;
    inode_list[used_inodes].flag = flag;
    used_inodes++;
	
	return used_inodes;
}

void testRead() {
    FILE *disk = fopen("/home/tarushroy/CSC360/a3/disk/vdisk", "rb+");

    int i;
    char buffer[BLOCK_SIZE];
    char *b = buffer;

    memset(b, 0, BLOCK_SIZE);
    readBlock(disk, 0, buffer);

    printf("Block 0:\n");
    for(i=0; i<BLOCK_SIZE; i++) {
        printf("%02X", buffer[i]);
    }
    printf("\n");

    memset(b, 0, BLOCK_SIZE);
    readBlock(disk, 1, buffer);

    printf("Block 1:\n");
    for(i=0; i<BLOCK_SIZE; i++) {
        printf("%02X", buffer[i]);
    }
    printf("\n");

    fclose(disk);
}