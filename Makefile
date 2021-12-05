
CC=gcc
#CFLAGS=-O0 -Wall -std=gnu11 -ggdb3 -fsanitize=undefined
CFLAGS=-Ofast -Wall -std=gnu11

OBJS = avlspeed.o avlhelper.o
TESTS = avltest_00 avltest_01 avltest_02 avltest_03 avltest_04 avltest_05 avltest_06 avltest_07 avltest_08

.PHONY: all clean

all: avlspeed $(TESTS)

%.o:%.c inline_avl.h avlhelper.h
	$(CC) -c $(CFLAGS) $(filter %.c,$^) -I. -o $@

avlspeed: $(OBJS)
	$(CC) $(CFLAGS) $^ -I. -o $@

avltest_00: avltest_00.c avlhelper.o
	$(CC) $(CFLAGS) $^ -I. -o $@

avltest_01: avltest_01.c avlhelper.o
	$(CC) $(CFLAGS) $^ -I. -o $@

avltest_02: avltest_02.c avlhelper.o
	$(CC) $(CFLAGS) $^ -I. -o $@

avltest_03: avltest_03.c avlhelper.o
	$(CC) $(CFLAGS) $^ -I. -o $@

avltest_04: avltest_04.c avlhelper.o
	$(CC) $(CFLAGS) $^ -I. -o $@

avltest_05: avltest_05.c avlhelper.o
	$(CC) $(CFLAGS) $^ -I. -o $@

avltest_06: avltest_06.c avlhelper.o
	$(CC) $(CFLAGS) $^ -I. -o $@

avltest_07: avltest_07.c avlhelper.o
	$(CC) $(CFLAGS) $^ -I. -o $@

avltest_08: avltest_08.c avlhelper.o
	$(CC) $(CFLAGS) $^ -I. -o $@

clean:
	rm -f *.o avlspeed $(TESTS)

