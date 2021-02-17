#ifndef FILE_H
#define FILE_H

void InitLLFS(void);

void createFile(char *filename, int flag);

void deleteFile(char *filename);

void writeToFile(char *pathname, char *buffer);

void readFile(char *pathname, char *buffer);

void fsck();

int createINODE(int blockNum, int flag);

void testRead();

#endif