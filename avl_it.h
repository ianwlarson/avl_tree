#pragma once

#include "avl.h"

#include "astack.h"

struct avl_it {
    struct avl_tree const*tree;
    void *stack_buffer;
    size_t buffer_size;
    astack_t m_stack;
    unsigned gen; 
    int backward;
};

int avl_it_next(struct avl_it *it, int *key, void **elem);

int avl_it_start(struct avl_it *it, struct avl_tree const*const tree, int const backward, void *const stack_buffer, size_t const buffer_size);

int verify_tree(struct avl_tree *tree);

// vim: filetype=c
