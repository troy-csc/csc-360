#ifndef DISK_H
#define DISK_H

#define BLOCK_SIZE 512
#define NUM_BLOCKS 4096

void writeBlock(FILE *disk, int blockNum, char *buffer);

void readBlock(FILE *disk, int blockNum, char *buffer);

void InitBlock_st(void);

void writeBlock_st(FILE *disk);

int findFreeBlock(void);

#endif