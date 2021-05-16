#include "avl_it.h"

#include <stdlib.h>
#include <assert.h>
#include <errno.h>

static inline int
max_int(int const a, int const b)
{
    return (a > b) ? a : b;
}

static inline int
get_height(e_avl_node const*const p_n)
{
    if (p_n == NULL) {
        return 0;
    }

    return p_n->height;
}

// Recursively descend the tree while ensuring that each stored value
// is correct, as well as that the tree is a legal AVL tree.
static int
recursive_check_height(e_avl_node *const n)
{
    if (n == NULL) {
        return 0;
    }

    int const l_height = recursive_check_height(n->lc);
    assert(l_height == get_height(n->lc));
    int const r_height = recursive_check_height(n->rc);
    assert(r_height == get_height(n->rc));
    int const balance = r_height - l_height;

    /* Ensure the keys are correctly sorted */
    if (n->lc != NULL) {
        assert(n->lc->key < n->key);
    }
    if (n->rc != NULL) {
        assert(n->rc->key > n->key);
    }

    assert((balance >= -1) && (balance <= 1));

    return 1 + max_int(l_height, r_height);
}

// Use a recursive function to examine the tree.
// Always returns 0, but will cause an assertion failure
// if the tree is invalid.
int
verify_tree(struct avl_tree *tree)
{
    struct avl_node *n = tree->m_top;
    if (n == NULL) {
        return 0;
    }

    int const height = recursive_check_height(n);

    assert(height == n->height);

    return 0;
}

// Returns -1 if the iterator was invalidated.
// Returns 1 when there are no more elements
// Returns 0 if succeeded on moving to next elem.
int
avl_it_next(struct avl_it *it, int *key, void **elem)
{
    // If the tree was modified, since the last call
    // to the iterator, return -1.
    if (it->gen != it->tree->m_gen) {
        errno = EINVAL;
        return -1;
    }

    struct avl_node *n = stack_pop(&it->m_stack);
    if (n == NULL) {
        return 1;
    }

    *key = n->key;
    *elem = (void *)((unsigned char *)n - it->tree->m_node_offset);

    int status = 0;
    if (it->backward) {
        n = n->lc;
        while (n != NULL) {
            status = stack_push(&it->m_stack, n);
            if (status == -1) {
                errno = ENOMEM;
                return -1;
            }
            n = n->rc;
        }
    } else {
        n = n->rc;
        while (n != NULL) {
            status = stack_push(&it->m_stack, n);
            if (status == -1) {
                errno = ENOMEM;
                return -1;
            }
            n = n->lc;
        }
    }

    return 0;
}

// Create an iterator for the tree.
int
avl_it_start(struct avl_it *it, struct avl_tree const*const tree, int const backward, void *const stack_buffer, size_t const buffer_size)
{
    it->tree = tree;
    it->gen = tree->m_gen;
    it->stack_buffer = stack_buffer;
    it->buffer_size = buffer_size;
    it->m_stack = stack_init(stack_buffer, buffer_size);
    it->backward = backward;

    struct avl_node *n = tree->m_top;
    int status = 0;
    while (n != NULL) {
        status = stack_push(&it->m_stack, n);
        if (status == -1) {
            errno = ENOMEM;
            return -1;
        }
        n = backward ? n->rc : n->lc;
    }

    return 0;
}
