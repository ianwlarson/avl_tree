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

#define DFOUND  0
#define DLEFT   1
#define DRIGHT  2

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
dive(e_avl_node *p_nd, astack_t *const p_stack, int const key)
{
    int status = DFOUND;
    for (;;) {
        status = stack_push(p_stack, p_nd);

        if (key < p_nd->key) {
            if (p_nd->lc == NULL) {
                status = DLEFT;
                break;
            } else {
                p_nd = p_nd->lc;
            }
        } else if (key > p_nd->key) {
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

    return status;
}

static int rebalance(struct avl_tree *tree, struct astack *stk);

static inline int
get_height(e_avl_node const*const p_n)
{
    if (p_n == NULL) {
        return 0;
    }

    return p_n->height;
}

static inline void
update_height(struct avl_node *const node)
{
    node->height = 1 + max_int(get_height(node->lc), get_height(node->rc));
}

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

#define ROT_BALANCED  0x00000000u
#define ROT_FIRST_L   0x00000001u
#define ROT_FIRST_R   0x00000002u
#define ROT_FMASK     (ROT_FIRST_L | ROT_FIRST_R)
#define ROT_SECND_L   0x00000004u
#define ROT_SECND_R   0x00000008u

static inline unsigned
find_case(e_avl_node const*const node)
{
    int const height_rc = get_height(node->rc);
    int const height_lc = get_height(node->lc);

    int const balance = height_rc - height_lc;

    unsigned rot_case = ROT_BALANCED;

    if (balance < -1 || balance > 1) {
        if (height_lc > height_rc) {
            rot_case |= ROT_FIRST_L;

            struct avl_node const*const child = node->lc;

            if (get_height(child->lc) < get_height(child->rc)) {
                rot_case |= ROT_SECND_R;
            }
        } else {
            rot_case |= ROT_FIRST_R;

            struct avl_node const*const child = node->rc;

            if (get_height(child->lc) > get_height(child->rc)) {
                rot_case |= ROT_SECND_L;
            }
        }
    }

    return rot_case;
}

// Adds an element to the tree. Returns 0 on success, -1 on failure.
int
avl_add(avl_tree_t *const tree, void *const p_obj, int const key, void *const stack_buffer, size_t const buffer_size)
{
    if (tree == NULL) {
        return -1;
    }

    e_avl_node *const add_node = (void *)((unsigned char *)p_obj + tree->m_node_offset);
    init_node(add_node, key);

    if (tree->m_size == 0) {
        tree->m_top = add_node;
    } else {

        astack_t l_stack = stack_init(stack_buffer, buffer_size);
        astack_t *const stack = &l_stack;
        if (l_stack.max_sz < avl_height(tree)) {
            errno = ENOMEM;
            return -1;
        }

        // Find the point of insertion into the tree
        int const rc = dive(tree->m_top, stack, key);

        e_avl_node *const parent = stack_peek(stack);
        if (rc == DLEFT) {
            parent->lc = add_node;
        } else if (rc == DRIGHT) {
            parent->rc = add_node;
        } else {
            errno = EEXIST;
            return -1;
        }

        rebalance(tree, stack);
    }

    ++tree->m_size;
    ++tree->m_gen;

    return 0;
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

void *
avl_rem(avl_tree_t *const tree, int const key, void *const stack_buffer, size_t const buffer_size)
{
    if (tree->m_size == 0) {
        return NULL;
    }

    astack_t l_stack = stack_init(stack_buffer, buffer_size);
    astack_t *const stack = &l_stack;
    if (l_stack.max_sz < avl_height(tree)) {
        errno = ENOMEM;
        return NULL;
    }

    e_avl_node *node = tree->m_top;
    int const dive_rc = dive(node, stack, key);
    if (dive_rc != DFOUND) {
        errno = ENOENT;
        return NULL;
    }

    // At this point, the stack contains an element with key
    // equal to `key`
    e_avl_node *const to_remove = stack_pop(stack);
    e_avl_node *const rem_parent = stack_peek(stack);

    void *const out = (void *)((unsigned char *)to_remove - tree->m_node_offset);

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
        /* Push the node we are removing onto the stack. We will replace it once
         * we find a node */
        stack_push(stack, to_remove);

        node = to_remove;

        /* One of the child nodes is not NULL, find a child node to replace the
         * candidate for deletion. */
        if (node->lc != NULL) {
            /* Find the largest keyed child in the left subtree */
            node = node->lc;
            for (;;) {
                stack_push(stack, node);
                if (node->rc == NULL) {
                    break;
                }
                node = node->rc;
            }
        } else {
            /* find the smallest keyed child in the right subtree */
            node = node->rc;
            stack_push(stack, node);

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
        }

        /* At this point, we have found the node to replace the node that we're deleting.
         * We have placed every node along that path onto our stack */

        e_avl_node *const replacement = stack_pop(stack);
        e_avl_node *const replace_parent = stack_peek(stack);

        /* make sure to keep children of replace_candidate */
        if (replacement->key < to_remove->key) {
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

        /* Find the placeholder we put on the stack and put our replacement there */
        __attribute__((unused)) bool found = false;
        for (int i = 0; i < stack->sz; ++i) {
            if (stack->data[i] == to_remove) {
                stack->data[i] = replacement;
                found = true;
                break;
            }
        }
        assert(found); // LCOV_EXCL_BR_LINE
    }

    /* Zero the node we removed */
    *to_remove = (e_avl_node) {};

    rebalance(tree, stack);

    tree->m_size--;
    tree->m_gen++;

    return out;
}

int
avl_height(struct avl_tree const*const tree)
{
    return get_height(tree->m_top);
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

static int
rebalance(avl_tree_t *tree, struct astack *const p_stack)
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

    return 0;
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
