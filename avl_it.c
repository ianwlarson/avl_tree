#include "avl_it.h"

#include <assert.h>

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
