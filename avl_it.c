#include "avl_it.h"

#include <assert.h>

// Recursively descend the tree while ensuring that each stored value
// is correct, as well as that the tree is a legal AVL tree.
static int
recursive_check_height(struct avl_node *n)
{
    if (n == NULL) {
        return 0;
    }

    int const l_height = recursive_check_height(n->lc);
    assert(l_height == get_height(n->lc));
    int const r_height = recursive_check_height(n->rc);
    assert(r_height == get_height(n->rc));
    int const balance = r_height - l_height;

    assert((balance >= -1) && (balance <= 1));

    return 1 + max(l_height, r_height);
}

// Use a recursive function to examine the tree.
// Always returns 0, but will cause an assertion failure
// if the tree is invalid.
int
verify_tree(struct avl_tree *tree)
{
    struct avl_node *n = tree->top;
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
    if (it->gen != it->tree->gen) {
        return -1;
    }

    struct avl_node *n = stack_pop(&it->stack);
    if (n == NULL) {
        return 1;
    }

    *key = n->key;
    *elem = n->elem;

    if (it->backward) {
        n = n->lc;
        while (n != NULL) {
            stack_push(&it->stack, n);
            n = n->rc;
        }
    } else {
        n = n->rc;
        while (n != NULL) {
            stack_push(&it->stack, n);
            n = n->lc;
        }
    }

    return 0;
}

// Create an iterator for the tree.
int
avl_it_start(struct avl_it *it, struct avl_tree const*const tree, int const backward)
{
    it->tree = tree;
    it->gen = tree->gen;
    it->stack.size = 0;
    it->backward = backward;

    struct avl_node *n = tree->top;
    while (n != NULL) {
        stack_push(&it->stack, n);
        n = backward ? n->rc : n->lc;
    }

    return 0;
}
