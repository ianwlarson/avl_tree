#pragma once

#include "inline_avl.h"

typedef struct my_type my_t;
struct my_type {
    e_avl_node ok;
    int my_key;
};

typedef struct my_key_type myk_t;
struct my_key_type {
    int my_key;
};

my_t *avl_my_add(avl_tree_t *tree, my_t *t);
my_t *avl_my_get(avl_tree_t const*tree, int key);
my_t *avl_my_rem(avl_tree_t *tree, int key);

