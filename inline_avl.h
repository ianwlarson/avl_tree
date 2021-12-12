
#ifndef INLINE_AVL_H
#define INLINE_AVL_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#ifndef assert
#define assert(x)
#endif

/*
 * Stack structure for allowing tree searching/backtracking.
 * p_stack->s[p_stack->size] refers to the empty entry at the top of the stack.
 *
 * AVL trees have a max height of 1.44 * log2(N)
 */

typedef struct astack astack_t;

struct astack {
    size_t sz;
    void *data[];
};

static inline astack_t*
stack_init(void *const buffer)
{
    astack_t *const o = buffer;
    o->sz = 0;
    return o;
}

__attribute__((always_inline))
static inline void **
stack_push(astack_t *const p_stack, void *const p_entry)
{
    assert(p_stack->sz <= 40);
    void **const o = &p_stack->data[p_stack->sz++];
    *o = p_entry;
    return o;
}


static inline void *
stack_pop(astack_t *const p_stack)
{
    if (p_stack->sz == 0) {
        return NULL;
    }
    return p_stack->data[--p_stack->sz];
}

static inline void *
stack_peek(astack_t const*const p_stack)
{
    if (p_stack->sz == 0) {
        return NULL;
    }
    return p_stack->data[p_stack->sz - 1];
}


/* embedded avl node */
typedef struct avl_node e_avl_node;

struct avl_node {
    e_avl_node *lc;
    e_avl_node *rc;
    int bf;
#if UINTPTR_MAX == 0xffffffffffffffffull
    // It's sort of pointless to include this but it's good to be explicit that
    // this field will be present. The implementation doesn't touch it so it
    // could be used to store extra information (maybe typing information or
    // something, on 64-bit)
    int reserved;
#endif
};

typedef struct avl_tree avl_tree_t;

struct avl_tree {
    e_avl_node *m_top; /* top of the tree */
    size_t m_size;
    unsigned m_gen; /* generation is used for iterators */
};

typedef int (*avlcmp_t)(e_avl_node const*, e_avl_node const*);
typedef int (*avlkeycmp_t)(void const*, e_avl_node const*);

static inline avl_tree_t
avl_tree_init(void)
{
    return (avl_tree_t) {
        .m_top = NULL,
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

static inline void
rotate_right(e_avl_node **const branch)
{
    /*
     * Right Rotate around &R->rc
     *
     *        R                R
     *       / \              / \
     *      x   a            x   b
     *         / \     ->       / \
     *        b   c            d   a
     *       / \                  / \
     *      d   e                e   c
     *
     */
    e_avl_node *const nd_a = *branch;
    e_avl_node *const nd_b = nd_a->lc;
    e_avl_node *const nd_e = nd_b->rc;

    *branch = nd_b;
    nd_b->rc = nd_a;
    nd_a->lc = nd_e;
}

static inline void
rotate_left(e_avl_node **const branch)
{
    /*
     * Left Rotate around &R->lc
     *
     *        R              R
     *       / \            / \
     *      a   x          c   x
     *     / \       ->   / \
     *    b   c          a   e
     *       / \        / \
     *      d   e      b   d
     *
     */
    e_avl_node *const nd_a = *branch;
    e_avl_node *const nd_c = nd_a->rc;
    e_avl_node *const nd_d = nd_c->lc;

    *branch = nd_c;
    nd_c->lc = nd_a;
    nd_a->rc = nd_d;
}

// Mutate functions
static inline e_avl_node *
avl_base_add(
    avl_tree_t *const tree,
    e_avl_node *const node,
    avlcmp_t const cmpfunc,
    void *const stack_buffer)
{
    node->lc = NULL;
    node->rc = NULL;
    node->bf = 0;

    // Exit early for the unlikely size == 0 case
    if (tree->m_size == 0) {
        tree->m_top = node;
        ++tree->m_size;
        ++tree->m_gen;
        return node;
    }

    astack_t *const stack = stack_init(stack_buffer);

    // Find the point of insertion into the tree
    e_avl_node *cursor = tree->m_top;

    for (;;) {
        (void)stack_push(stack, cursor);

        int const lcmp = cmpfunc(node, cursor);
        if (lcmp < 0) {
            if (cursor->lc == NULL) {
                cursor->lc = node;
                cursor->bf--;
                break;
            } else {
                cursor = cursor->lc;
            }
        } else if (lcmp > 0) {
            if (cursor->rc == NULL) {
                cursor->rc = node;
                cursor->bf++;
                break;
            } else {
                cursor = cursor->rc;
            }
        } else {
            return cursor;
        }
    }

    ++tree->m_size;
    ++tree->m_gen;

    // At this point, node has been added to the tree and every node along the
    // way is on the stack.
    e_avl_node *z = node;
    e_avl_node *x = stack_pop(stack);
    for (;;) {
        assert(x != NULL);

        if (x->bf == 2 || x->bf == -2) {
            // The node was made unbalanced! It can be balanced by rotation,
            // then we're done!
            e_avl_node **branch = &tree->m_top;
            e_avl_node *y = stack_peek(stack);
            if (y != NULL) {
                branch = (y->rc == x) ? &y->rc : &y->lc;
            }

            if (x->rc == z) {
                if (z->bf < 0) {
                    // c's left child goes to x, right child goes to z
                    e_avl_node *const c = z->lc;
                    rotate_right(&x->rc);
                    rotate_left(branch);

                    if (c->bf == 0) {
                        z->bf = 0;
                        x->bf = 0;
                    } else if (c->bf == -1) {
                        // left child was higher
                        x->bf = 0;
                        z->bf = 1;
                    } else {
                        // right child was higher
                        assert(c->bf == 1);
                        x->bf = -1;
                        z->bf = 0;
                    }

                    c->bf = 0;
                } else {
                    rotate_left(branch);

                    z->bf = 0;
                    x->bf = 0;
                }
            } else {
                if (z->bf > 0) {
                    // c's left child goes to z, right child goes to x
                    e_avl_node *const c = z->rc;
                    rotate_left(&x->lc);
                    rotate_right(branch);

                    if (c->bf == 0) {
                        z->bf = 0;
                        x->bf = 0;
                    } else if (c->bf == -1) {
                        // left child was higher
                        x->bf = 1;
                        z->bf = 0;
                    } else {
                        // right child was higher
                        assert(c->bf == 1);
                        x->bf = 0;
                        z->bf = -1;
                    }

                    c->bf = 0;
                } else {
                    rotate_right(branch);

                    z->bf = 0;
                    x->bf = 0;
                }
            }

            break;
        } else if (x->bf == 0) {
            // The change in height was absorbed! We're done!
            break;
        } else {
            // Our balance has become 1 or -1, we must continue up the tree.
            z = x;
            x = stack_pop(stack);
            if (x == NULL) break;

            if (x->rc == z) {
                x->bf++;
            } else {
                x->bf--;
            }
        }
    }

    return node;
}

// Gets the pointer associated with a key.
__attribute__((pure))
static inline e_avl_node *
avl_base_get(avl_tree_t const*const tree, void const*const key, avlkeycmp_t const cmpfunc)
{
    e_avl_node *node = tree->m_top;

    // Iterate down the tree structure, without using a stack
    // (gets don't require keeping track of the path.)
    for (;;) {
        if (node == NULL) {
            return NULL;
        }

        int const lcmp = cmpfunc(key, node);
        if (lcmp < 0) {
            node = node->lc;
        } else if (lcmp > 0) {
            node = node->rc;
        } else {
            return node;
        }
    }
}

static inline e_avl_node *
avl_base_rem(
    avl_tree_t *const tree,
    void const*const key,
    avlkeycmp_t const cmpfunc,
    void *const stack_buffer)
{
    if (tree->m_size == 0) {
        return NULL;
    }

    astack_t *const stack = stack_init(stack_buffer);

    // Find the point of insertion into the tree
    e_avl_node *cursor = tree->m_top;
    for (;;) {
        (void)stack_push(stack, cursor);

        int const lcmp = cmpfunc(key, cursor);
        if (lcmp < 0) {
            if (cursor->lc == NULL) {
                return NULL;
            } else {
                cursor = cursor->lc;
            }
        } else if (lcmp > 0) {
            if (cursor->rc == NULL) {
                return NULL;
            } else {
                cursor = cursor->rc;
            }
        } else {
            break;
        }
    }

    tree->m_size--;
    tree->m_gen++;

    e_avl_node *z = stack_pop(stack);
    e_avl_node *const to_remove = z;
    e_avl_node *x = stack_peek(stack);

    if (z->lc == NULL && z->rc == NULL) {
        // The node we are removing is a leaf node. Remove references to it
        if (x == NULL) {
            assert(tree->m_size == 0);
            tree->m_top = NULL;
            return to_remove;

        } else {
            if (x->lc == z) {
                x->lc = NULL;
                x->bf++;
            } else {
                assert(x->rc == z);
                x->rc = NULL;
                x->bf--;
            }
        }
    } else {
        // Push the node we are removing onto the stack. We will replace it
        // once we find a substitute.
        void **const rem_stack_ptr = stack_push(stack, z);

        e_avl_node *sub = z;

        // One of the child nodes is not NULL, find a child node to replace the
        // candidate for deletion.
        if (sub->lc != NULL) {
            // Find the largest keyed child in the left subtree
            sub = sub->lc;
            for (;;) {
                if (sub->rc == NULL) {
                    break;
                }
                (void)stack_push(stack, sub);

                sub = sub->rc;
            }
            e_avl_node *const subp = stack_peek(stack);

            sub->bf = z->bf;
            if (subp != z) {
                // sub is the right child of subp
                subp->bf--;
                subp->rc = sub->lc;

                sub->lc = z->lc;
                sub->rc = z->rc;
            } else {
                // sub is the left child of z, so we effectively move it up the
                // tree.
                sub->bf++;
                sub->rc = z->rc;
                // Leave sub->lc intact
            }

        } else {

            sub = sub->rc;
            // sub is the right child of z, so we effectively move it up the
            // tree.
            sub->bf = z->bf - 1;

            assert(sub->lc == NULL); // LCOV_EXCL_BR_LINE
            /* Node cannot have a left child because it would
             * create the illegal tree below.
             *
             *            a
             *             \
             *              b
             *             /
             *            c
             * If `c` existed and the tree was balanced, `a` would have a left
             * child that would be preferred for replacement.
             */

            sub->lc = z->lc;
            // leave sub->rc intact
        }

        // Replace the element in the stack with the ptr
        *rem_stack_ptr = sub;

        if (x == NULL) {
            tree->m_top = sub;
        } else {
            if (x->lc == z) {
                x->lc = sub;
            } else {
                x->rc = sub;
            }
        }
    }
    z = NULL;
    x = stack_pop(stack);

    // At this point z has been removed from the tree, and x is the first node
    // above it that is potentially unbalanced.

    for (;;) {
        assert(x != NULL);

        if (x->bf == 1 || x->bf == -1) {
            // Change in height was absorbed.
            break;
        } else if (x->bf == 2 || x->bf == -2) {
            // The node was made unbalanced! It can be balanced by rotation,
            // then we may be done.
            e_avl_node **branch = &tree->m_top;
            e_avl_node *y = stack_peek(stack);
            if (y != NULL) {
                branch = (y->rc == x) ? &y->rc : &y->lc;
            }
            int zb = 0;

            if (x->bf == 2) {
                // The height of the left subtree was decreased by 1
                z = x->rc;
                zb = z->bf;
                if (zb < 0) {
                    e_avl_node *const c = z->lc;
                    rotate_right(&x->rc);
                    rotate_left(branch);

                    if (c->bf == 0) {
                        z->bf = 0;
                        x->bf = 0;
                    } else if (c->bf == -1) {
                        // left child was higher
                        x->bf = 0;
                        z->bf = 1;
                    } else {
                        // right child was higher
                        assert(c->bf == 1);
                        x->bf = -1;
                        z->bf = 0;
                    }

                    c->bf = 0;
                    x = c;
                } else {
                    rotate_left(branch);
                    if (zb == 0) {
                        // x gets z's left child
                        x->bf = 1;
                        z->bf = -1;
                    } else {
                        z->bf = 0;
                        x->bf = 0;
                    }
                    x = z;
                }
            } else {
                // The height of the right subtree was decreased by 1
                z = x->lc;
                zb = z->bf;
                if (zb > 0) {
                    e_avl_node *const c = z->rc;
                    rotate_left(&x->lc);
                    rotate_right(branch);

                    if (c->bf == 0) {
                        z->bf = 0;
                        x->bf = 0;
                    } else if (c->bf == -1) {
                        // left child was higher
                        x->bf = 1;
                        z->bf = 0;
                    } else {
                        // right child was higher
                        assert(c->bf == 1);
                        x->bf = 0;
                        z->bf = -1;
                    }

                    c->bf = 0;
                    x = c;
                } else {
                    rotate_right(branch);
                    if (zb == 0) {
                        // x gets z's right child
                        x->bf = -1;
                        z->bf = 1;

                    } else {
                        z->bf = 0;
                        x->bf = 0;
                    }
                    x = z;
                }
            }
            // After the rotation, x is set to y's new child to ensure that
            // moving up the tree is correct.

            // TODO: Is this exit condition correct?
            if (zb == 0) break;

        } else {
            assert(x->bf == 0);
            // Our balance has become 0, we must continue retracing up the
            // tree.
        }

        z = x;
        x = stack_pop(stack);
        if (x == NULL) break;

        // adjust x's bf to account for the decreased height of subtree z
        if (x->rc == z) {
            x->bf--;
        } else {
            x->bf++;
        }
    }

    return to_remove;
}

#endif /* INLINE_AVL_H */
