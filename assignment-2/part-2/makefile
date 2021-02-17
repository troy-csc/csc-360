CC = gcc
CFLAGS = -g -Wall -Werror -Wno-deprecated-declarations -pthread

all: dataPar

dataPar: dataPar.c dataPar.h
	$(CC) $(CFLAGS) -o dataPar dataPar.c

.PHONY: clean

clean:
	rm -f dataPar *.o