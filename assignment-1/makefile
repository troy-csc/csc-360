CC=gcc
CFLAGS=-Wall -Werror

all: SEEsh

SEEsh: SEEsh.o
	$(CC) $(CFLAGS) -o SEEsh SEEsh.o

SEEsh.o: SEEsh.c SEEsh.h
	$(CC) $(CFLAGS) -c SEEsh.c -o SEEsh.o