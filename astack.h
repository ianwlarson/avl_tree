#pragma once

#include <stdlib.h>
#include <assert.h>

/*
 * Stack structure for allowing tree searching/backtracking.
 * p_stack->s[p_stack->size] refers to the empty entry at the top of the stack.
 *
 * AVL trees have a max height of 1.44 * log2(N)
 */

typedef struct astack astack_t;

struct astack {
    void **data;
    size_t max_sz;
    size_t sz;
};

static inline astack_t
stack_init(void *const buffer, size_t const buffer_size)
{
    return (astack_t) {
        .data = buffer,
        .max_sz = buffer_size / sizeof(void *),
        .sz = 0,
    };
}

static inline void
stack_push(astack_t *const p_stack, void *const p_entry)
{
    p_stack->data[p_stack->sz++] = p_entry;
}


static inline void *
stack_pop(astack_t *const p_stack)
{
    if (p_stack->sz == 0) {
        return NULL;
    }
    return p_stack->data[--p_stack->sz];
}

static inline void *
stack_peek(astack_t const*const p_stack)
{
    if (p_stack->sz == 0) {
        return NULL;
    }
    return p_stack->data[p_stack->sz - 1];
}

