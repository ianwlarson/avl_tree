
CC=gcc
CFLAGS=-Ofast -Wall -std=gnu11

OBJS = avlspeed.o avlhelper.o

.PHONY: all clean

all: avlspeed

%.o:%.c inline_avl.h avlhelper.h
	$(CC) -c $(CFLAGS) $(filter %.c,$^) -I. -o $@

avlspeed: $(OBJS)
	$(CC) $(CFLAGS) $^ -I. -o $@

clean:
	rm -f *.o avlspeed

