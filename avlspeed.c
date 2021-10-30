
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>

#include "avlhelper.h"

static inline unsigned
xorshift32(unsigned *const p_rng)
{
    unsigned x = *p_rng;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *p_rng = x;
    return x;
}

#define NUM_LOOPS 100000
#define NUM_OBJS (1<<17)
#define NUM_INNER_LOOP 100

int
main(void)
{
    printf("NUM_OBJS %d\n", NUM_OBJS);
    printf("NUM_INNER_LOOP %d\n", NUM_INNER_LOOP);
    unsigned rng = time(NULL);
    //unsigned rng = 0xdeadbeefu;

    avl_tree_t tree = avl_tree_init();

    my_t *objs = malloc(sizeof(*objs) * NUM_OBJS);
    for (int i = 0; i < NUM_OBJS; ++i) {
        // Randomly put a non-negative key in all the objects
        objs[i].my_key = (int)(xorshift32(&rng) & 0xefffffffu);
    }

    struct timespec start, end;
    uint64_t get_ns = 0;
    uint64_t add_ns = 0;
    uint64_t rem_ns = 0;


    for (int i = 0; i < NUM_OBJS; ++i) {
        // Put all the objects into the tree
        my_t *a = avl_my_add(&tree, &objs[i]);
        while (a != &objs[i]) {
            objs[i].my_key = (int)(xorshift32(&rng) & 0xefffffffu);
            a = avl_my_add(&tree, &objs[i]);
        }
    }

    my_t **ptrs = malloc(sizeof(*ptrs) * NUM_INNER_LOOP);

    for (int i = 0; i < NUM_LOOPS; ++i) {
        clock_gettime(CLOCK_REALTIME, &start);
        for (int j = 0; j < NUM_INNER_LOOP; ++j) {
            unsigned const idx = xorshift32(&rng) % NUM_OBJS;
            my_t *g = avl_my_get(&tree, objs[idx].my_key);
            assert(g == &objs[idx]);
        }
        clock_gettime(CLOCK_REALTIME, &end);

        get_ns += (end.tv_sec - start.tv_sec)*UINT64_C(1000000000) + (end.tv_nsec - start.tv_nsec);

        clock_gettime(CLOCK_REALTIME, &start);
        unsigned const start_idx = xorshift32(&rng) % NUM_OBJS;
        for (int j = 0; j < NUM_INNER_LOOP; ++j) {
            unsigned const idx = (start_idx + j) % NUM_OBJS;
            my_t *const e = avl_my_rem(&tree, objs[idx].my_key);
            assert(e == &objs[idx]);
            ptrs[j] = e;
        }
        clock_gettime(CLOCK_REALTIME, &end);

        rem_ns += (end.tv_sec - start.tv_sec)*UINT64_C(1000000000) + (end.tv_nsec - start.tv_nsec);

        clock_gettime(CLOCK_REALTIME, &start);
        for (int j = 0; j < NUM_INNER_LOOP; ++j) {
            unsigned const idx = (start_idx + j) % NUM_OBJS;
            my_t *const e = avl_my_add(&tree, ptrs[j]);
            assert(e == &objs[idx]);
        }
        clock_gettime(CLOCK_REALTIME, &end);
        add_ns += (end.tv_sec - start.tv_sec)*UINT64_C(1000000000) + (end.tv_nsec - start.tv_nsec);
    }

    double const divisor = 1.0 * NUM_LOOPS * NUM_INNER_LOOP;
    printf("Ran test with a tree of size %d\n", NUM_OBJS);
    printf("Average time to get a node: %f nanoseconds\n", 1.0 * get_ns / divisor);
    printf("Average time to add a node: %f nanoseconds\n", 1.0 * add_ns / divisor);
    printf("Average time to remove a node: %f nanoseconds\n", 1.0 * rem_ns / divisor);

    free(objs);
    free(ptrs);

    return 0;
}
