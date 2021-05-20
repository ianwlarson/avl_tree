
#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "avl.h"

static inline unsigned
xorshift32(unsigned *const p_rng)
{
    unsigned x = *p_rng;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *p_rng = x;
    return x;
}

typedef struct my_type my_t;
struct my_type {
    e_avl_node ok;
    int my_key;
};

typedef struct my_key_type myk_t;
struct my_key_type {
    int my_key;
};

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

static inline my_t *
avl_my_add(avl_tree_t *const tree, my_t *const t)
{
    void *stack[45];
    e_avl_node *const o = avl_base_add(tree, &t->ok, mycmp, stack);
    return (void *)((unsigned char *)o - offsetof(my_t, ok));
}

static inline my_t *
avl_my_get(avl_tree_t const*const tree, int const key)
{
    myk_t const k = {
        .my_key = key,
    };
    e_avl_node *const o = avl_base_get(tree, &k, mykeycmp);
    return (void *)((unsigned char *)o - offsetof(my_t, ok));
}

static inline my_t *
avl_my_rem(avl_tree_t *const tree, int const key)
{
    myk_t const k = {
        .my_key = key,
    };
    void *stack[45];
    e_avl_node *const o = avl_base_rem(tree, &k, mykeycmp, stack);
    return (void *)((unsigned char *)o - offsetof(my_t, ok));
}


// Recursively descend the tree while ensuring that each stored value
// is correct, as well as that the tree is a legal AVL tree.
static unsigned
recursive_check_height(e_avl_node const*const n, avlcmp_t const cmpfunc)
{
    if (n == NULL) {
        return 0;
    }

    int const l_height = recursive_check_height(n->lc, cmpfunc);
    assert(l_height == avl_node_height(n->lc));
    int const r_height = recursive_check_height(n->rc, cmpfunc);
    assert(r_height == avl_node_height(n->rc));
    int const balance = r_height - l_height;

    /* Ensure the keys are correctly sorted */
    if (n->lc != NULL) {
        int lcmp = cmpfunc(n->lc, n);
        assert(lcmp < 0);
    }
    if (n->rc != NULL) {
        int lcmp = cmpfunc(n->rc, n);
        assert(lcmp > 0);
    }

    assert((balance >= -1) && (balance <= 1));

    unsigned h = (l_height > r_height) ? l_height : r_height;
    return 1 + h;
}

// Use a recursive function to examine the tree.
// Always returns 0, but will cause an assertion failure
// if the tree is invalid.
static int
verify_tree(avl_tree_t const*const tree, avlcmp_t const cmpfunc)
{
    e_avl_node *const n = tree->m_top;
    if (n == NULL) {
        return 0;
    }

    int const height = recursive_check_height(n, cmpfunc);

    assert(height == avl_node_height(n));

    return 0;
}

int
main(void)
{
    unsigned rng = time(NULL);
    //unsigned rng = 0xdeadbeefu;

    avl_tree_t tree = avl_tree_init();

    int max_h = 0;
    size_t max_sz = 0;
    for (int i = 0; i < 2000000; ++i) {
        int const key = (int)(xorshift32(&rng) & 0xffffffu);
        if (avl_my_get(&tree, key) == NULL) {
            my_t *const m = malloc(sizeof(*m));
            m->my_key = key;
            my_t *const res = avl_my_add(&tree, m);
            assert(res == m);
        }
        else {
            //printf("Key %d already exists\n", key);
            //printf("remove %u\n", key.my_key);
            free(avl_my_rem(&tree, key));
        }
        //verify_tree(&tree, mycmp);
        if (avl_height(&tree) > max_h) {
            max_h = avl_height(&tree);
        }
        if (avl_size(&tree) > max_sz) {
            max_sz = avl_size(&tree);
        }
    }
    printf("Max height was %d\n", max_h);
    printf("Max size was %zu\n", max_sz);

    return 0;
}
