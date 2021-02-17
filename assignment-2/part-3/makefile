CC = gcc
CFLAGS = -g -Wall -Werror -Wno-deprecated-declarations -pthread

all: diningOut

diningOut: diningOut.c diningOut.h
	$(CC) $(CFLAGS) -o diningOut diningOut.c

.PHONY: clean

clean:
	rm -f diningOut *.o