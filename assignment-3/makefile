CC:=gcc
CFLAGS:=-g -Wall -Werror

all: test01 test02 disk/disk.o io/File.o

test01: apps/test01.c io/File.o
    $(CC) $(CFLAGS) -o apps/$@ $^ disk/disk.c

test02: apps/test02.c io/File.o
    $(CC) $(CFLAGS) -o apps/$@ $^ disk/disk.c

disk/disk.o: disk/disk.c disk/disk.h
    $(CC) $(CFLAGS) -c -o $@ $<

io/File.o: io/File.c io/File.h
    $(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
    rm apps/test01
    rm apps/test02
    rm io/File.o
    rm disk/disk.o
