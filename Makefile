
CC=gcc
CFLAGS=-Ofast -Wall -std=gnu11

.PHONY: all clean

all: avlspeed

avlspeed: avlspeed.c | avl.h
	$(CC) $(CFLAGS) $^ -I. -o $@

clean:
	rm -f *.o avlspeed

