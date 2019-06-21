#include "avl_it.h"

#include <assert.h>

int
avl_it_next(struct avl_itr *it, void **elem)
{
    if (it->gen != it->tree->gen) {
        return -1;
    }

    struct avl_node *n = stack_pop(&it->stack);
    if (n == NULL) {
        return 1;
    }

    *elem = n->elem;

    n = n->rc;
    while (n != NULL) {
        stack_push(&it->stack, n);
        n = n->lc;
    }

    return 0;
}

int
avl_it_prev(struct avl_itr *it, void **elem)
{
    if (it->gen != it->tree->gen) {
        return -1;
    }

    return 0;
}

int
avl_it_start(struct avl_itr *it, struct avl_tree *tree)
{
    it->tree = tree;
    it->gen = tree->gen;
    it->stack.size = 0;

    struct avl_node *n = tree->top;
    while (n != NULL) {
        stack_push(&it->stack, n);
        n = n->lc;
    }

    return 0;
}
