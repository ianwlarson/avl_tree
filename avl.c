
#include "avl.h"

#include <stdbool.h>
#include <assert.h>

#include <stdio.h>

#define ASTACK_MAX (36)
#define ASTACK_INIT {.size = 0}
struct astack {
    void *s[ASTACK_MAX];
    int size;
};

static inline void
stack_push(struct astack *stack, void *const entry)
{
    assert(stack->size < ASTACK_MAX);
    stack->s[stack->size] = entry;
    stack->size++;
}


static inline void *
stack_pop(struct astack *stack)
{
    if (stack->size == 0) {
        return NULL;
    }
    stack->size--;
    return stack->s[stack->size];
}


static inline void *
stack_peek(struct astack *stack)
{
    if (stack->size == 0) {
        return NULL;
    }
    return stack->s[stack->size - 1];
}


void rotate_right(struct avl_node **branch, struct avl_node *node);
void rotate_left(struct avl_node **branch, struct avl_node *node);

static int dive(struct avl_node *node, struct astack *stk, int const key);
static int rebalance(struct avl_tree *tree, struct astack *stk);

#define CRASH_IF(condition) assert(!(condition))

static inline int
max(int const a, int const b)
{
    return (a > b) ? a : b;
}

static inline int
get_height(struct avl_node const*const node)
{
    if (node == NULL) {
        return 0;
    }

    return node->height;
}

// The balance of any node is the height of the right sub-tree
// minus the height of the left sub-tree.
static inline int
get_balance(struct avl_node const*const node)
{
    if (node == NULL) {
        return 0;
    }

    return get_height(node->rc) - get_height(node->lc);
}

static inline void
update_height(struct avl_node *const node)
{
    node->height = 1 + max(get_height(node->lc), get_height(node->rc));
}

struct avl_node *
create_new_node(void *const elem, int const key)
{
    struct avl_node *n = malloc(sizeof(*n));
    CRASH_IF(n == NULL);

    n->elem = elem;
    n->lc = NULL;
    n->rc = NULL;
    n->key = key;
    n->height = 1;

    return n;
}

void
delete_node(struct avl_node *const node)
{
    free(node);
}

enum direction {
    LEFT,
    RIGHT,
};


enum avl_rotate_case {
    BALANCED,
    LEFT_LEFT,
    LEFT_RIGHT,
    RIGHT_RIGHT,
    RIGHT_LEFT,
};

static inline enum avl_rotate_case
find_case(struct avl_node const*const node)
{
    int const balance = get_balance(node);

    if (balance >= -1 && balance <= 1) {
        return BALANCED;
    }

    if (get_height(node->lc) > get_height(node->rc)) {
        struct avl_node const*const child = node->lc;

        // When the heights are equal, default to the easier case
        if (get_height(child->lc) >= get_height(child->rc)) {
            return LEFT_LEFT;
        } else {
            return LEFT_RIGHT;
        }
    } else {
        struct avl_node const*const child = node->rc;

        // When the heights are equal, default to the easier case
        if (get_height(child->lc) > get_height(child->rc)) {
            return RIGHT_LEFT;
        } else {
            return RIGHT_RIGHT;
        }
    }
}

int
avl_add(struct avl_tree *tree, void *const elem, int const key)
{
    struct astack stack = ASTACK_INIT;

    struct avl_node *node = tree->top;

    // Find the point of insertion into the tree
    int const rc = dive(node, &stack, key);

    switch (rc) {
        case 0:
            tree->top = create_new_node(elem, key);
            tree->size++;
            return 0;
            break;
        case 1:
            node = stack_peek(&stack);
            node->lc = create_new_node(elem, key);
            break;
        case 2:
            node = stack_peek(&stack);
            node->rc = create_new_node(elem, key);
            break;
        case 3:
            return -1;
            break;
        default:
            CRASH_IF(true);
            break;
    }

    rebalance(tree, &stack);

    tree->size++;

    return 0;
}

void *
avl_get(struct avl_tree const*const tree, int const key)
{
    struct astack stack = ASTACK_INIT;
    struct avl_node *const node = tree->top;
    if (node == NULL) {
        return NULL;
    }

    int const rc = dive(node, &stack, key);
    if (rc == 3) {
        struct avl_node *n = stack_pop(&stack);
        return n->elem;
    }

    return NULL;
}

void *
avl_rem(struct avl_tree *tree, int const key)
{
    struct astack stack = ASTACK_INIT;

    struct avl_node *node = tree->top;
    int const dive_rc = dive(node, &stack, key);
    if (dive_rc != 3) {
        return NULL;
    }

    // At this point, the stack contains an element with key
    // equal to `key`
    node = stack_pop(&stack);

    // Store the value to be returned, as well as the node
    void *const out = node->elem;
    struct avl_node *const candidate = node;

    // If the node is a leaf node, peek the parent,
    // remove references to the candidate, and free the candidate.
    if (node->lc == NULL && node->rc == NULL) {
        node = stack_peek(&stack);

        // If there's no parent, the candidate is the last
        // node.
        if (node == NULL) {
            CRASH_IF(stack.size > 0);
            tree->top = NULL;
        } else {
            if (node->lc == candidate) {
                node->lc = NULL;
            } else if (node->rc == candidate) {
                node->rc = NULL;
            } else {
                CRASH_IF(true);
            }
        }
        delete_node(candidate);
    } else {
        // Push the node back onto the stack, because it will need
        // to be balanced.
        stack_push(&stack, node);

        // One of the child nodes is not NULL, find a child node
        // to replace the candidate for deletion.
        if (node->lc != NULL) {
            node = node->lc;
            // Find the largest left child
            for (;;) {
                stack_push(&stack, node);
                if (node->rc == NULL) {
                    break;
                }
                node = node->rc;
            }
        } else if (node->rc != NULL) {
            node = node->rc;
            // Find the smallest right child
            for (;;) {
                stack_push(&stack, node);
                if (node->lc == NULL) {
                    break;
                }
                node = node->lc;
            }
        } else {
            CRASH_IF(true);
        }

        // At this point, we found the node to replace the node that
        // we're deleting, and every node along the path is on the stack.
        struct avl_node *const new_cand = stack_pop(&stack);
        struct avl_node *const parent = stack_peek(&stack);

        // Keep any children new_cand has.
        if (new_cand->key < candidate->key) {
            // new_cand is largest child in left sub-tree
            // We know new_cand has no right children, because
            // they would be larger than it.
            if (parent->lc == new_cand) {
                parent->lc = new_cand->lc;
            } else {
                parent->rc = new_cand->lc;
            }

        } else {
            // new_cand is smallest child in right sub-tree
            // We know new_cand has no left children.
            if (parent->lc == new_cand) {
                parent->lc = new_cand->rc;
            } else {
                parent->rc = new_cand->rc;
            }

        }

        candidate->elem = new_cand->elem;
        candidate->key = new_cand->key;
        delete_node(new_cand);
    }

    rebalance(tree, &stack);

    tree->size--;

    return out;
}

int
avl_height(struct avl_tree const*const tree)
{
    return get_height(tree->top);
}

void
rotate_right(struct avl_node **branch, struct avl_node *node)
{
    CRASH_IF(*branch != node);
    struct avl_node *const left = node->lc;
    struct avl_node *const left_right = left->rc;
    *branch = left;
    node->lc = left_right;
    left->rc = node;
    update_height(node);
    update_height(left);
}

void
rotate_left(struct avl_node **branch, struct avl_node *node)
{
    CRASH_IF(*branch != node);
    struct avl_node *const r = node->rc;
    struct avl_node *const rl = r->lc;
    *branch = r;
    node->rc = rl;
    r->lc = node;
    update_height(node);
    update_height(r);
}

// Search down the tree structure, keeping track of our traversal
// in the stack.
//
// Return value is based on the value on the top of the stack
//
// 0 - Empty sub-tree, No node were added to the stack.
// 1 - The node The left child is NULL
// 2 - The right child is NULL
// 3 - The node matches the `key`
static int
dive(struct avl_node *node, struct astack *stk, int const key)
{
    if (node == NULL) {
        return 0;
    }

    for (;;) {
        stack_push(stk, node);
        if (key < node->key) {
            // If we find a NULL child, the node doesn't exist
            if (node->lc == NULL) {
                return 1;
            } else {
                node = node->lc;
            }
        } else if (key > node->key) {
            // If we find a NULL child, the node doesn't exist
            if (node->rc == NULL) {
                return 2;
            } else {
                node = node->rc;
            }
        } else {
            // We have found an element with key `key`
            return 3;
        }
    }
}

static int
rebalance(struct avl_tree *tree, struct astack *stk)
{
    //printf("Starting rebalance.\n");
    struct avl_node *node = stack_pop(stk);
    // Traverse back up the tree, rebalancing and adjusting height
    while (node != NULL) {

        update_height(node);
        enum avl_rotate_case const rot = find_case(node);
        struct avl_node *parent = stack_peek(stk);
        struct avl_node **branch = NULL;
        struct avl_node **branch2 = NULL;

        // Get a reference to the pivot point for rotation.
        if (parent == NULL) {
            branch = &tree->top;
        } else {
            if (node == parent->lc) {
                branch = &parent->lc;
            } else if (node == parent->rc) {
                branch = &parent->rc;
            } else {
                CRASH_IF(true);
            }
        }

        //printf("The case is: ");
        switch (rot) {
        case BALANCED:
            //printf("balanced.\n");
            break;
        case LEFT_LEFT:
            rotate_right(branch, node);
            //printf("left left\n");
            break;
        case LEFT_RIGHT:
            branch2 = &node->lc;
            rotate_left(branch2, *branch2);
            rotate_right(branch, *branch);
            //printf("left right\n");
            break;
        case RIGHT_RIGHT:
            //printf("right right\n");
            rotate_left(branch, node);
            break;
        case RIGHT_LEFT:
            branch2 = &node->rc;
            rotate_right(branch2, *branch2);
            rotate_left(branch, *branch);
            //printf("right left\n");
            break;
        default:
            CRASH_IF(true);
            break;
        }

        node = stack_pop(stk);
    }
    //printf("Finished rebalance.\n");

    return 0;
}