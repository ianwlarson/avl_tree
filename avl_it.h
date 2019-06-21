#pragma once

#include "avl.h"

struct avl_it {
    struct avl_tree const*tree;
    struct astack stack;
    unsigned gen; 
    int backward;
};

int avl_it_next(struct avl_it *it, int *key, void **elem);

int avl_it_start(struct avl_it *it, struct avl_tree const*const tree, int const backward);

int verify_tree(struct avl_tree *tree);

