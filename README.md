
# AVL Tree

An AVL tree is a flavour of _Rank-Balanced Binary Search Trees_.
It is more rigidly balanced than a Red-Black tree, but indeed every AVL tree may be
coloured red-black.

An AVL tree has an advantage over a Red-Black tree for use cases where insertions
and deletions are infrequent relative to number of element access.

## Implementation

### Details
This particular tree is implemented without using recursion or parent pointers
in the node structures. In order to do this, a buffer must be passed to the
functions to use as a stack when traversing the tree.

This makes the API slightly more awkward, but in reality it's trivial to allocate a sufficiently
large buffer for any reasonably sized tree.

The benefits are enourmous:
1. The per-node memory cost is reduced by a pointer as no parent pointer is
   required.
2. The constant cost of updating is reduced as there is 33% less pointers to
   update when rebalancing.

### Example Usage

Let's create a custom avl tree for an object type:

```c
#include "inline_avl.h"

// Define the base type that contains an embedded node
typedef struct my_type my_t;
struct my_type {
    e_avl_node ok;
    int my_key;
};

// Define a key type that can be used to locate entries in the tree
typedef struct my_key_type myk_t;
struct my_key_type {
    int my_key;
};

// Define a static inline pure function that compares two nodes
__attribute__((pure))
static inline int
mycmp(e_avl_node const*const ln, e_avl_node const*const rn)
{
    my_t const*const l = (void *)((unsigned char *)ln - offsetof(my_t, ok));
    my_t const*const r = (void *)((unsigned char *)rn - offsetof(my_t, ok));
    if (l->my_key < r->my_key) {
        return -1;
    } else if (l->my_key > r->my_key) {
        return 1;
    } else {
        return 0;
    }
}

// Define a static inline pure function that compares a key and a node
__attribute__((pure))
static inline int
mykeycmp(void const*const key, e_avl_node const*const rn)
{
    myk_t const*const l = key;
    my_t const*const r = (void *)((unsigned char *)rn - offsetof(my_t, ok));
    if (l->my_key < r->my_key) {
        return -1;
    } else if (l->my_key > r->my_key) {
        return 1;
    } else {
        return 0;
    }
}

// Define a function that wraps the `avl_base_add` api by handling the
// converting the object type (strongly typed) to the inner node, as well
// as providing the comparison function and stack.
__attribute__((flatten))
static inline my_t *
avl_my_add(avl_tree_t *const tree, my_t *const t)
{
    void *stack[45];
    e_avl_node *const o = avl_base_add(tree, &t->ok, mycmp, stack);
    return (void *)((unsigned char *)o - offsetof(my_t, ok));
}

// Define a function that wraps `avl_base_get` by constructing a key object
// and passing in the key comparison function.
__attribute__((pure,flatten))
static inline my_t *
avl_my_get(avl_tree_t const*const tree, int const key)
{
    myk_t const k = {
        .my_key = key,
    };
    e_avl_node *const o = avl_base_get(tree, &k, mykeycmp);
    if (o == NULL) {
        return NULL;
    } else {
        return (void *)((unsigned char *)o - offsetof(my_t, ok));
    }
}

// Define a function that wraps `avl_base_rem` by constructing a key object,
// then providing the comparison function and the stack.
__attribute__((flatten))
static inline my_t *
avl_my_rem(avl_tree_t *const tree, int const key)
{
    myk_t const k = {
        .my_key = key,
    };
    void *stack[45];
    e_avl_node *const o = avl_base_rem(tree, &k, mykeycmp, stack);
    if (o == NULL) {
        return NULL;
    } else {
        return (void *)((unsigned char *)o - offsetof(my_t, ok));
    }
}
```
