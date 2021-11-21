
#ifndef INLINE_AVL_H
#define INLINE_AVL_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

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
    void **data;
    size_t sz;
};

static inline astack_t
stack_init(void *const buffer)
{
    return (astack_t) {
        .data = buffer,
        .sz = 0,
    };
}

__attribute__((always_inline))
static inline void **
stack_push(astack_t *const p_stack, void *const p_entry)
{
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
    int height;
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
static inline int
avl_node_height(e_avl_node const*const p_n)
{
    if (p_n == NULL) {
        return 0;
    }

    return p_n->height;
}

__attribute__((pure))
static inline int
avl_height(avl_tree_t const*const tree)
{
    return avl_node_height(tree->m_top);
}

__attribute__((pure))
static inline size_t
avl_size(avl_tree_t const*const p_tree)
{
    return p_tree->m_size;
}


#define DFOUND  0
#define DLEFT   1
#define DRIGHT  2
#define DERROR  3

/* Search down the tree structure, keeping track of our traversal
 * in the stack.
 * Return value is based on the value on the top of the stack.
 *
 * - Cases where the node doesn't exist in the tree
 *   DLEFT  - The left child of the node on the top of the stack is NULL
 *   DRIGHT - The right child of the node on the top of the stack is NULL
 *
 * - Cases where the node is found
 *   DFOUND - A node with a matching key was found in the tree
 */
static inline int
dive(
    e_avl_node *p_nd,
    e_avl_node const*const lhs,
    avlcmp_t const cmpfunc,
    astack_t *const p_stack)
{
    int status = DERROR;
    for (;;) {
        (void)stack_push(p_stack, p_nd);

        int const lcmp = cmpfunc(lhs, p_nd);
        if (lcmp < 0) {
            if (p_nd->lc == NULL) {
                status = DLEFT;
                break;
            } else {
                p_nd = p_nd->lc;
            }
        } else if (lcmp > 0) {
            if (p_nd->rc == NULL) {
                status = DRIGHT;
                break;
            } else {
                p_nd = p_nd->rc;
            }
        } else {
            // We found a matching element
            status = DFOUND;
            break;
        }
    }
    assert(status != DERROR); // LCOV_EXCL_BR_LINE

    return status;
}

static inline int
divek(
    e_avl_node *p_nd,
    void const*const lhs,
    avlkeycmp_t const cmpfunc,
    astack_t *const p_stack)
{
    int status = DERROR;
    for (;;) {
        (void)stack_push(p_stack, p_nd);

        int const lcmp = cmpfunc(lhs, p_nd);
        if (lcmp < 0) {
            if (p_nd->lc == NULL) {
                status = DLEFT;
                break;
            } else {
                p_nd = p_nd->lc;
            }
        } else if (lcmp > 0) {
            if (p_nd->rc == NULL) {
                status = DRIGHT;
                break;
            } else {
                p_nd = p_nd->rc;
            }
        } else {
            // We have found an element with key `key`
            status = DFOUND;
            break;
        }
    }
    assert(status != DERROR); // LCOV_EXCL_BR_LINE

    return status;
}


static inline void
update_height(struct avl_node *const node)
{
    int const height_lc = avl_node_height(node->lc);
    int const height_rc = avl_node_height(node->rc);
    int const maxheight = (height_rc > height_lc) ? height_rc : height_lc;
    node->height = 1 + maxheight;
}

/*
 * Apply a rotation around pivot point `node`
 *
 * `branch` is the pointer to `node` which must be updated.
 *
 */
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

    update_height(nd_a);
    update_height(nd_b);
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

    update_height(nd_a);
    update_height(nd_c);
}


#define ROT_BALANCED  0x00000000u
#define ROT_FIRST_L   0x00000001u
#define ROT_FIRST_R   0x00000002u
#define ROT_FMASK     (ROT_FIRST_L | ROT_FIRST_R)
#define ROT_SECND_L   0x00000004u
#define ROT_SECND_R   0x00000008u

__attribute__((pure))
static inline unsigned
find_case(e_avl_node const*const node)
{
    int const height_rc = avl_node_height(node->rc);
    int const height_lc = avl_node_height(node->lc);

    int const balance = height_rc - height_lc;

    unsigned rot_case = ROT_BALANCED;

    if (balance < -1 || balance > 1) {
        if (height_lc > height_rc) {
            rot_case |= ROT_FIRST_L;

            e_avl_node const*const child = node->lc;

            if (avl_node_height(child->lc) < avl_node_height(child->rc)) {
                rot_case |= ROT_SECND_R;
            }
        } else {
            rot_case |= ROT_FIRST_R;

            e_avl_node const*const child = node->rc;

            if (avl_node_height(child->lc) > avl_node_height(child->rc)) {
                rot_case |= ROT_SECND_L;
            }
        }
    }

    return rot_case;
}

static inline void
rebalance(avl_tree_t *const tree, struct astack *const p_stack)
{
    e_avl_node *node = stack_pop(p_stack);

    /* Traverse back up the tree, rebalancing and adjusting height */
    while (node != NULL) {

        update_height(node);
        unsigned const rot = find_case(node);
        e_avl_node *const parent = stack_peek(p_stack);

        /* branch is the pivot point to rotate through:
         *
         *  ->  \                        \  <-
         *       a                        b
         *        \           ->         / \
         *         b                    a   c
         *          \
         *           c
         */
        struct avl_node **branch = NULL;

        /* Get a reference to the pivot point for rotation. */
        if (parent == NULL) {
            branch = &tree->m_top;
        } else {
            if (node == parent->lc) {
                branch = &parent->lc;
            } else {
                branch = &parent->rc;
            }
        }

        if (rot != ROT_BALANCED) {
            if ((rot & ROT_FMASK) == ROT_FIRST_L) {
                /* left subtree has greater height */

                if (rot & ROT_SECND_R) {
                    rotate_left(&node->lc);
                }
                rotate_right(branch);
            } else {
                /* right subtree has greater height */

                if (rot & ROT_SECND_L) {
                    rotate_right(&node->rc);
                }
                rotate_left(branch);
            }
        }

        node = stack_pop(p_stack);
    }
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
    node->height = 1;

    if (tree->m_size == 0) {
        tree->m_top = node;
    } else {

        astack_t l_stack = stack_init(stack_buffer);
        astack_t *const stack = &l_stack;

        // Find the point of insertion into the tree
        int const rc = dive(tree->m_top, node, cmpfunc, stack);

        e_avl_node *const parent = stack_peek(stack);
        if (rc == DLEFT) {
            parent->lc = node;
        } else if (rc == DRIGHT) {
            parent->rc = node;
        } else {
            // We found a node that matches exactly
            return parent;
        }

        rebalance(tree, stack);
    }

    ++tree->m_size;
    ++tree->m_gen;

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

    astack_t l_stack = stack_init(stack_buffer);
    astack_t *const stack = &l_stack;

    e_avl_node *node = tree->m_top;
    int const dive_rc = divek(node, key, cmpfunc, stack);
    if (dive_rc != DFOUND) {
        return NULL;
    }

    // At this point, the stack contains an element with key
    // equal to `key`
    e_avl_node *const to_remove = stack_pop(stack);
    e_avl_node *const rem_parent = stack_peek(stack);

    if (to_remove->lc == NULL && to_remove->rc == NULL) {
        /* The node we are removing is a leaf node. Remove references to it */
        if (rem_parent == NULL) {
            tree->m_top = NULL;
        } else {
            if (rem_parent->lc == to_remove) {
                rem_parent->lc = NULL;
            } else {
                rem_parent->rc = NULL;
            }
        }
    } else {
        /* Push the node we are removing onto the stack. We will replace it
         * once we find a node */
        void **const rem_stack_ptr = stack_push(stack, to_remove);

        node = to_remove;
        int replace_case = DERROR;

        /* One of the child nodes is not NULL, find a child node to replace the
         * candidate for deletion. */
        if (node->lc != NULL) {
            /* Find the largest keyed child in the left subtree */
            node = node->lc;
            for (;;) {
                (void)stack_push(stack, node);
                if (node->rc == NULL) {
                    break;
                }
                node = node->rc;
            }
            replace_case = DLEFT;
        } else {
            /* find the smallest keyed child in the right subtree */
            node = node->rc;
            (void)stack_push(stack, node);

            // We do not loop and descend to try to find a better replacement
            // in this case.

            assert(node->lc == NULL); // LCOV_EXCL_BR_LINE
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
            replace_case = DRIGHT;
        }

        /* At this point, we have found the node to replace the node that we're deleting.
         * We have placed every node along that path onto our stack */

        e_avl_node *const replacement = stack_pop(stack);
        e_avl_node *const replace_parent = stack_peek(stack);

        /* make sure to keep children of replace_candidate */
        if (replace_case == DLEFT) {
            /* new_cand is largest child in left sub-tree of the node we are
             * removing. We know new_cand has no right children, because they
             * would be larger than it, and we would have preferred them */
            if (replace_parent->lc == replacement) {
                replace_parent->lc = replacement->lc;
            } else {
                replace_parent->rc = replacement->lc;
            }
            assert(replacement->rc == NULL); // LCOV_EXCL_BR_LINE

        } else {
            /* new_cand is the smallest child in the right sub-tree of the node
             * we are removing */
            assert(replacement != replace_parent->lc); // LCOV_EXCL_BR_LINE
            /* `new_cand` cannot be a left child,
             * because the tree below is illegal.
             *
             *            a
             *             \
             *              b
             *             /
             *            c
             * If the candidate for replacement was the left child `c`, then
             * `a` must have a left child, which would be preferred for
             * replacement.
             *
             * similarly, the candidate for replacement cannot have a left
             * child as it would be preferred for replacement
             */
            assert(replacement->lc == NULL); // LCOV_EXCL_BR_LINE
            replace_parent->rc = replacement->rc;
        }

        /* swap out the node we are removing with the replacement candidate */
        replacement->rc = to_remove->rc;
        replacement->lc = to_remove->lc;
        if (rem_parent == NULL) {
            tree->m_top = replacement;
        } else {
            if (rem_parent->rc == to_remove) {
                rem_parent->rc = replacement;
            } else {
                rem_parent->lc = replacement;
            }
        }

        /* Replace the element in the stack with the ptr */
        *rem_stack_ptr = replacement;
    }

    /* Zero some fields of the node we removed */
    to_remove->lc = NULL;
    to_remove->rc = NULL;
    to_remove->height = 0;

    rebalance(tree, stack);

    tree->m_size--;
    tree->m_gen++;

    return to_remove;
}

#endif /* INLINE_AVL_H */
