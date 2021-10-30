
#include "avlhelper.h"

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

__attribute__((flatten))
my_t *
avl_my_add(avl_tree_t *const tree, my_t *const t)
{
    void *stack[45];
    e_avl_node *const o = avl_base_add(tree, &t->ok, mycmp, stack);
    return (void *)((unsigned char *)o - offsetof(my_t, ok));
}

__attribute__((flatten))
my_t *
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

__attribute__((flatten))
my_t *
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

