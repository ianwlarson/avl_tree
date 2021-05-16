#pragma once

#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

/* embedded avl node */
typedef struct avl_node e_avl_node;

struct avl_node {
    e_avl_node *lc;
    e_avl_node *rc;
    int key;
    int height;
};

typedef struct avl_tree avl_tree_t;

struct avl_tree {
    e_avl_node *m_top; /* top of the tree */
    size_t m_node_offset; /* offset into the object to find the e_avl_node */
    size_t m_size;
    unsigned m_gen; /* generation is used for iterators */
};

static inline void
avl_tree_init(avl_tree_t *const p_tree, size_t const p_offset)
{
    *p_tree = (avl_tree_t) {
        .m_top = NULL,
        .m_node_offset = p_offset,
        .m_size = 0,
        .m_gen = 0,
    };
}

__attribute__((pure))
static inline size_t
avl_size(avl_tree_t const*const p_tree)
{
    return p_tree->m_size;
}

// Mutate functions
int   avl_add(struct avl_tree *tree, void *const elem, int const key, void *const stack_buffer, size_t const buffer_size);
void *avl_rem(struct avl_tree *tree, int const key, void *const stack_buffer, size_t const buffer_size);

// Read-only functions
__attribute__((pure))
void *avl_get(struct avl_tree const*const tree, int const key);

__attribute__((pure))
int avl_height(struct avl_tree const*const tree);

int avl_min_key(struct avl_tree const*const tree, int *key);
int avl_max_key(struct avl_tree const*const tree, int *key);

// vim: filetype=c
