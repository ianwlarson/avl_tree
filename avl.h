#pragma once

#include <stdlib.h>

#define AVL_TREE_INIT {.top = NULL, .size = 0}

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
};

// Mutate functions
int   avl_add(struct avl_tree *tree, void *const elem, int const key);
void *avl_rem(struct avl_tree *tree, int const key);

// Read-only functions
void *avl_get(struct avl_tree const*const tree, int const key);
int   avl_height(struct avl_tree const*const tree);


__attribute__((weak)) struct avl_node *create_new_node(void *const elem, int const key);
__attribute__((weak)) void delete_node(struct avl_node *const node);
