#include "avl_it.h"

#include <assert.h>

// Returns -1 if the iterator was invalidated.
// Returns 1 when there are no more elements
// Returns 0 if succeeded on moving to next elem.
int
avl_it_next(struct avl_itr *it, int *key, void **elem)
{
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

int
avl_it_start(struct avl_itr *it, struct avl_tree *tree, int const backward)
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
