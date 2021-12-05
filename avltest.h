
#ifndef AVLTEST_H
#define AVLTEST_H

static inline my_t *
nd2t(e_avl_node *const nd)
{
    if (nd == NULL) return NULL;

    return (void *)((unsigned char *)nd - offsetof(my_t, ok));
}

static inline my_t *
leftc(my_t *const m)
{
    return nd2t(m->ok.lc);
}
static inline my_t *
rightc(my_t *const m)
{
    return nd2t(m->ok.rc);
}

static inline int
KEY(my_t const*const m)
{
    return m->my_key;
}

static inline int
BF(my_t const*const m)
{
    return m->ok.bf;
}

static inline my_t *
TOP(avl_tree_t *const tree)
{
    return nd2t(tree->m_top);
}

#endif

