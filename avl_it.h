#pragma once

#include "avl.h"

struct avl_itr {
    struct avl_tree *tree;
    struct astack stack;
    unsigned gen; 
};

int avl_it_next(struct avl_itr *it, void **elem);
int avl_it_prev(struct avl_itr *it, void **elem);

int avl_it_start(struct avl_itr *it, struct avl_tree *tree);

