
CC=gcc
CFLAGS=-Ofast -Wall -std=gnu11

.PHONY: all clean

all: avlspeed

avlspeed: avlspeed.c inline_avl.h
	$(CC) $(CFLAGS) $(filter %.c,$^) -I. -o $@

clean:
	rm -f *.o avlspeed

