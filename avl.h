#pragma once

#include <stdlib.h>
#include <assert.h>

#define AVL_TREE_INIT {.top = NULL, .size = 0, .gen = 0}

struct avl_node {
    void *elem;
    struct avl_node *lc; // left child
    struct avl_node *rc; // right child
    int key;
    int height;
};

struct avl_tree {
    struct avl_node *top;
    int size;
    unsigned gen;
};

// Mutate functions
int   avl_add(struct avl_tree *tree, void *const elem, int const key);
void *avl_rem(struct avl_tree *tree, int const key);

// Read-only functions
void *avl_get(struct avl_tree const*const tree, int const key);
int   avl_height(struct avl_tree const*const tree);

int   avl_min_key(struct avl_tree const*const tree, int *key);
int   avl_max_key(struct avl_tree const*const tree, int *key);


__attribute__((weak)) struct avl_node *create_new_node(void *const elem, int const key);
__attribute__((weak)) void delete_node(struct avl_node *const node);

#define ASTACK_MAX (36)
#define ASTACK_INIT {.size = 0}
struct astack {
    void *s[ASTACK_MAX];
    int size;
};

static inline void
stack_push(struct astack *stack, void *const entry)
{
    assert(stack->size < ASTACK_MAX);
    stack->s[stack->size] = entry;
    stack->size++;
}


static inline void *
stack_pop(struct astack *stack)
{
    if (stack->size == 0) {
        return NULL;
    }
    stack->size--;
    return stack->s[stack->size];
}


static inline void *
stack_peek(struct astack *stack)
{
    if (stack->size == 0) {
        return NULL;
    }
    return stack->s[stack->size - 1];
}
