#include <stdbool.h>
#include <errno.h>
#include <assert.h>

#include "avl.h"
#include "astack.h"


static inline int
max_int(int const a, int const b)
{
    return (a > b) ? a : b;
}

static int rebalance(struct avl_tree *tree, struct astack *stk);

static inline void
init_node(e_avl_node *const p_node, unsigned const p_key)
{
    *p_node = (e_avl_node) {
        .lc = NULL,
        .rc = NULL,
        .height = 1,
        .key = p_key,
    };
}

// Adds an element to the tree. Returns 0 on success, -1 on failure.
int
avl_add(avl_tree_t *const tree, void *const p_obj, int const key, void *const stack_buffer, size_t const buffer_size)
{
    e_avl_node *const add_node = (void *)((unsigned char *)p_obj + tree->m_node_offset);
}

// Gets the pointer associated with a key.
void *
avl_get(avl_tree_t const*const tree, int const key)
{
    e_avl_node *node = tree->m_top;

    // Iterate down the tree structure, without using a stack
    // (gets don't require keeping track of the path.)
    for (;;) {
        if (node == NULL) {
            return NULL;
        }

        if (key < node->key) {
            node = node->lc;
        } else if (key > node->key) {
            node = node->rc;
        } else {
            return (void *)((unsigned char *)node - tree->m_node_offset);
        }
    }
}



/*
 * Find the minimum key present in the tree.
 *
 * Returns -1 if the tree is empty
 *
 */
int
avl_min_key(struct avl_tree const*const tree, int *key)
{
    e_avl_node *n = tree->m_top;
    if (n == NULL) {
        return -1;
    }

    while (n->lc != NULL) {
        n = n->lc;
    }

    *key = n->key;

    return 0;
}

/*
 * Find the maximum key present in the tree.
 *
 * Returns -1 if the tree is empty.
 *
 */
int 
avl_max_key(struct avl_tree const*const tree, int *key)
{
    e_avl_node *n = tree->m_top;
    if (n == NULL) {
        return -1;
    }

    while (n->rc != NULL) {
        n = n->rc;
    }

    *key = n->key;

    return 0;
}
