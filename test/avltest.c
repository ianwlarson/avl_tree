
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <errno.h>

#include "avl.h"
#include "avl_it.h"

/* Three different objects with the node at different offests into it */
typedef struct {
    e_avl_node nd;
    unsigned data1[16];
    unsigned data2[16];
    int key;
} test_object1;

static inline void
init_obj1(test_object1 *const p_obj)
{
    for (int i = 0; i < sizeof(p_obj->data1) / sizeof(*p_obj->data1); ++i) {
        p_obj->data1[i] = 0xdeadbeefu;
    }
    for (int i = 0; i < sizeof(p_obj->data2) / sizeof(*p_obj->data2); ++i) {
        p_obj->data2[i] = 0xdeadbeefu;
    }
}

static inline int
verify_obj1(test_object1 const*const p_obj)
{
    for (int i = 0; i < sizeof(p_obj->data1) / sizeof(*p_obj->data1); ++i) {
        if (p_obj->data1[i] != 0xdeadbeefu) {
            return 1;
        }
    }
    for (int i = 0; i < sizeof(p_obj->data2) / sizeof(*p_obj->data2); ++i) {
        if (p_obj->data2[i] != 0xdeadbeefu) {
            return 1;
        }
    }

    return 0;
}

typedef struct {
    unsigned data1[16];
    e_avl_node nd;
    unsigned data2[16];
    int key;
} test_object2;

static inline void
init_obj2(test_object2 *const p_obj)
{
    for (int i = 0; i < sizeof(p_obj->data1) / sizeof(*p_obj->data1); ++i) {
        p_obj->data1[i] = 0xdeadbeefu;
    }
    for (int i = 0; i < sizeof(p_obj->data2) / sizeof(*p_obj->data2); ++i) {
        p_obj->data2[i] = 0xdeadbeefu;
    }
}

static inline int
verify_obj2(test_object2 const*const p_obj)
{
    for (int i = 0; i < sizeof(p_obj->data1) / sizeof(*p_obj->data1); ++i) {
        if (p_obj->data1[i] != 0xdeadbeefu) {
            return 1;
        }
    }
    for (int i = 0; i < sizeof(p_obj->data2) / sizeof(*p_obj->data2); ++i) {
        if (p_obj->data2[i] != 0xdeadbeefu) {
            return 1;
        }
    }

    return 0;
}

typedef struct {
    unsigned data1[16];
    unsigned data2[16];
    e_avl_node nd;
} test_object3;

static inline void
init_obj3(test_object3 *const p_obj)
{
    for (int i = 0; i < sizeof(p_obj->data1) / sizeof(*p_obj->data1); ++i) {
        p_obj->data1[i] = 0xdeadbeefu;
    }
    for (int i = 0; i < sizeof(p_obj->data2) / sizeof(*p_obj->data2); ++i) {
        p_obj->data2[i] = 0xdeadbeefu;
    }
}

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

/* Return a random integer in range (-max, max) */
static inline int
randnum(unsigned *const p_rng, int const max)
{
    int o = xorshift32(p_rng);
    return o % max;
}

static void
test_basic_functionality(void **state)
{
    (void) state;

    // Test passing a null tree to avl_add returns a negative value.
    int rc = avl_add(NULL, NULL, 1, NULL, 0);
    assert_true(rc < 0);

    struct avl_tree tree;
    avl_tree_init(&tree, offsetof(test_object1, nd));
    size_t const ssize = sizeof(void *) * 64;
    void *const sbuf = test_malloc(ssize);

    test_object1 obj;
    init_obj1(&obj);

    // Test that an element can be added
    rc = avl_add(&tree, &obj, 1, sbuf, ssize);
    assert_return_code(rc, 0);
    assert_true(avl_size(&tree) == 1);

    // Test that an element with an existing key won't be added
    rc = avl_add(&tree, &obj, 1, sbuf, ssize);
    assert_int_equal(rc, -1);
    assert_true(avl_size(&tree) == 1);

    // Test than a non-existing elements aren't found.
    void *const p1 = avl_get(&tree, 22);
    assert_null(p1);
    void *const p12 = avl_get(&tree, -22);
    assert_null(p12);

    // Test that the element can be found
    void *const p2 = avl_get(&tree, 1);
    assert_non_null(p2);
    assert_ptr_equal(p2, &obj);
    assert_int_equal(avl_height(&tree), 1);

    // Test that trying to remove a non-existing
    // element won't do anything
    void *const p3 = avl_rem(&tree, 0, sbuf, ssize);
    assert_null(p3);
    assert_int_equal(avl_size(&tree), 1);
    assert_int_equal(avl_height(&tree), 1);

    // Test that an existing element can be removed
    void *const p4 = avl_rem(&tree, 1, sbuf, ssize);
    assert_non_null(p4);
    assert_ptr_equal(p2, p4);
    assert_true(avl_size(&tree) == 0);
    assert_int_equal(avl_height(&tree), 0);

    assert_int_equal(verify_obj1(&obj), 0);

    test_free(sbuf);
}

static void
test_zero_size_tree(void **state)
{
    (void)state;

    struct avl_tree tree;
    avl_tree_init(&tree, offsetof(test_object1, nd));
    size_t const ssize = sizeof(void *) * 64;
    void *const sbuf = test_malloc(ssize);

    assert_null(avl_get(&tree, 22));
    assert_null(avl_rem(&tree, 11, sbuf, ssize));

    assert_int_equal(avl_height(&tree), 0);
    assert_int_equal(verify_tree(&tree), 0);

    int key = 0;
    assert_int_equal(avl_min_key(&tree, &key), -1);
    assert_int_equal(avl_max_key(&tree, &key), -1);

    test_free(sbuf);
}

static void
test_small_stack(void **state)
{
    (void)state;

    struct avl_tree tree;
    avl_tree_init(&tree, offsetof(test_object1, nd));
    size_t const ssize = sizeof(void *) * 64;
    void *const sbuf = test_malloc(ssize);

    void *small_stack[1];

    test_object1 *objs = test_malloc(sizeof(*objs) * 50);
    assert_non_null(objs);
    int status = 0;
    for (int i = 0; i < 49; ++i) {
        status = avl_add(&tree, &objs[i], i, sbuf, ssize);
        assert_int_equal(status, 0);
    }

    status = avl_add(&tree, &objs[49], 49, small_stack, sizeof(small_stack));
    assert_int_equal(status, -1);
    assert_int_equal(errno, ENOMEM);

    void *out = avl_rem(&tree, 1, small_stack, sizeof(small_stack));
    assert_ptr_equal(out, NULL);
    assert_int_equal(errno, ENOMEM);

    test_free(sbuf);
    test_free(objs);
}

static void
test_basic_right_right_rotate(void **state)
{
    (void)state;

    struct avl_tree tree;
    avl_tree_init(&tree, offsetof(test_object2, nd));
    size_t const ssize = sizeof(void *) * 64;
    void *const sbuf = test_malloc(ssize);

    test_object2 *objs = test_malloc(sizeof(*objs) * 3);
    for (int i = 0; i < 3; ++i) {
        objs[i].key = i;
        init_obj2(&objs[i]);
    }

    /* Add 3 nodes to the tree and verify that the height indicates the tree was
     * balanced */
    avl_add(&tree, &objs[0], 0, sbuf, ssize);
    avl_add(&tree, &objs[1], 1, sbuf, ssize);
    avl_add(&tree, &objs[2], 2, sbuf, ssize);

    assert_int_equal(avl_height(&tree), 2);
    assert_true(avl_size(&tree) == 3);

    test_object2 *obj = NULL;
    obj = avl_rem(&tree, 1, sbuf, ssize);
    assert_int_equal(obj->key, 1);
    obj = avl_rem(&tree, 0, sbuf, ssize);
    assert_int_equal(obj->key, 0);
    obj = avl_rem(&tree, 2, sbuf, ssize);
    assert_int_equal(obj->key, 2);

    assert_int_equal(avl_height(&tree), 0);
    assert_int_equal(avl_size(&tree), 0);

    for (int i = 0; i < 3; ++i) {
        assert_int_equal(verify_obj2(&objs[i]), 0);
    }

    test_free(sbuf);
    test_free(objs);
}

static void
test_basic_multiple_nodes(void **state)
{
    (void)state;

    struct avl_tree tree;
    avl_tree_init(&tree, offsetof(test_object2, nd));
    size_t const ssize = sizeof(void *) * 64;
    void *const sbuf = test_malloc(ssize);
    int status = 0;

    /* This should produce the following tree
     *                 3
     *                /  \
     *               /    \
     *              1      7
     *             / \    / \
     *            0   2  5   8
     *                  / \   \
     *                 4   6   9
     */

    for (int i = 0; i < 10; ++i) {
        test_object2 *const obj2 = test_malloc(sizeof(*obj2));
        init_obj2(obj2);
        obj2->key = i;
        int const rc = avl_add(&tree, obj2, i, sbuf, ssize);
        assert_return_code(rc, 0);
    }

    int key;
    status = avl_min_key(&tree, &key);
    assert_int_equal(status, 0);
    assert_int_equal(key, 0);
    status = avl_max_key(&tree, &key);
    assert_int_equal(status, 0);
    assert_int_equal(key, 9);

    assert_true(avl_size(&tree) == 10);
    assert_int_equal(avl_height(&tree), 4);
    assert_int_equal(tree.m_top->key, 3);

    assert_int_equal(tree.m_top->lc->key, 1);
    assert_int_equal(tree.m_top->lc->lc->key, 0);
    assert_int_equal(tree.m_top->lc->rc->key, 2);

    assert_int_equal(tree.m_top->rc->key, 7);
    assert_int_equal(tree.m_top->rc->lc->key, 5);
    assert_int_equal(tree.m_top->rc->rc->key, 8);

    assert_int_equal(tree.m_top->rc->lc->lc->key, 4);
    assert_int_equal(tree.m_top->rc->lc->rc->key, 6);

    assert_int_equal(tree.m_top->rc->rc->rc->key, 9);
    assert_null(tree.m_top->rc->rc->lc);

    for (int i = 0; i < 10; ++i) {
        test_object2 *const obj2 = avl_rem(&tree, i, sbuf, ssize);
        assert_non_null(obj2);
        assert_int_equal(obj2->key, i);
        assert_int_equal(verify_obj2(obj2), 0);
        test_free(obj2);
    }

    assert_true(avl_size(&tree) == 0);
    assert_int_equal(avl_height(&tree), 0);

    test_free(sbuf);
}

// Testing a sequence based off of a known valid
// implementation on the internet
static void
test_known_sequence(void **state)
{
    (void)state;

    struct avl_tree tree;
    avl_tree_init(&tree, offsetof(test_object2, nd));
    size_t const ssize = sizeof(void *) * 64;
    void *const sbuf = test_malloc(ssize);

    test_object2 *objs = test_malloc(sizeof(*objs) * 16);
    for (int i = 0; i < 16; ++i) {
        init_obj2(&objs[i]);
        objs[i].key = i;
    }

    avl_add(&tree, &objs[1], 1, sbuf, ssize);
    avl_add(&tree, &objs[9], 9, sbuf, ssize);
    avl_add(&tree, &objs[2], 2, sbuf, ssize);
    avl_add(&tree, &objs[8], 8, sbuf, ssize);
    avl_add(&tree, &objs[3], 3, sbuf, ssize);
    avl_add(&tree, &objs[7], 7, sbuf, ssize);

    assert_int_equal(tree.m_top->key, 3);
    assert_int_equal(avl_height(&tree), 3);

    test_object2 *l_obj = avl_rem(&tree, 3, sbuf, ssize);
    assert_int_equal(l_obj->key, 3);

    assert_int_equal(tree.m_top->key, 2);
    assert_int_equal(avl_height(&tree), 3);

    l_obj = avl_rem(&tree, 2, sbuf, ssize);
    assert_int_equal(l_obj->key, 2);

    assert_int_equal(tree.m_top->key, 8);
    assert_int_equal(avl_height(&tree), 3);

    avl_add(&tree, &objs[5], 5, sbuf, ssize);
    assert_int_equal(tree.m_top->key, 8);
    assert_int_equal(tree.m_top->lc->key, 5);
    assert_int_equal(tree.m_top->lc->lc->key, 1);
    assert_int_equal(tree.m_top->lc->rc->key, 7);
    assert_int_equal(avl_height(&tree), 3);

    avl_add(&tree, &objs[10], 10, sbuf, ssize);
    avl_add(&tree, &objs[6], 6, sbuf, ssize);
    assert_int_equal(tree.m_top->key, 8);
    assert_int_equal(avl_height(&tree), 4);

    l_obj = avl_rem(&tree, 8, sbuf, ssize);
    assert_int_equal(l_obj->key, 8);

    assert_int_equal(tree.m_top->key, 7);
    assert_int_equal(avl_height(&tree), 3);

    l_obj = avl_rem(&tree, 9, sbuf, ssize);
    assert_int_equal(l_obj->key, 9);

    l_obj = avl_rem(&tree, 10, sbuf, ssize);
    assert_int_equal(l_obj->key, 10);

    assert_int_equal(tree.m_top->key, 5);
    assert_int_equal(avl_height(&tree), 3);

    l_obj = avl_rem(&tree, 1, sbuf, ssize);
    assert_int_equal(l_obj->key, 1);

    assert_int_equal(tree.m_top->key, 6);
    assert_int_equal(avl_height(&tree), 2);

    l_obj = avl_rem(&tree, 5, sbuf, ssize);
    assert_int_equal(l_obj->key, 5);
    l_obj = avl_rem(&tree, 6, sbuf, ssize);
    assert_int_equal(l_obj->key, 6);

    assert_int_equal(tree.m_top->key, 7);
    assert_int_equal(avl_height(&tree), 1);

    l_obj = avl_rem(&tree, 7, sbuf, ssize);
    assert_int_equal(l_obj->key, 7);
    assert_int_equal(avl_size(&tree), 0);

    for (int i = 0; i < 16; ++i) {
        verify_obj2(&objs[i]);
    }
    test_free(objs);
    test_free(sbuf);
}

static void
test_left_right_rotation(void **state)
{
    (void)state;
    struct avl_tree tree;
    avl_tree_init(&tree, offsetof(test_object2, nd));
    size_t const ssize = sizeof(void *) * 64;
    void *const sbuf = test_malloc(ssize);

    test_object2 *objs = test_malloc(sizeof(*objs) * 20);
    for (int i = 0; i < 16; ++i) {
        init_obj2(&objs[i]);
        objs[i].key = i;
    }

    avl_add(&tree, &objs[13], 13, sbuf, ssize);
    avl_add(&tree, &objs[10], 10, sbuf, ssize);
    avl_add(&tree, &objs[15], 15, sbuf, ssize);
    avl_add(&tree, &objs[5], 5, sbuf, ssize);
    avl_add(&tree, &objs[11], 11, sbuf, ssize);
    avl_add(&tree, &objs[16], 16, sbuf, ssize);
    avl_add(&tree, &objs[4], 4, sbuf, ssize);
    avl_add(&tree, &objs[6], 6, sbuf, ssize);

    /*
     *                13
     *               /  \
     *              /    \
     *             10     15
     *            /  \      \
     *           5    11     16
     *          / \
     *         4   6
     */

    assert_int_equal(tree.m_top->key, 13);
    assert_int_equal(tree.m_top->lc->key, 10);
    assert_int_equal(tree.m_top->rc->key, 15);

    assert_int_equal(tree.m_top->lc->lc->key, 5);
    assert_int_equal(tree.m_top->lc->rc->key, 11);

    assert_int_equal(tree.m_top->lc->lc->lc->key, 4);
    assert_int_equal(tree.m_top->lc->lc->rc->key, 6);

    assert_int_equal(tree.m_top->rc->rc->key, 16);
    /*
     * Add 7 transforms it to:
     *                13
     *               /  \
     *              /    \
     *             6     15
     *            / \      \
     *           5   10     16
     *          /   /  \
     *         4   7    11
     */

    avl_add(&tree, &objs[7], 7, sbuf, ssize);

    assert_int_equal(tree.m_top->key, 13);
    assert_int_equal(tree.m_top->lc->key, 6);
    assert_int_equal(tree.m_top->rc->key, 15);

    assert_int_equal(tree.m_top->lc->lc->key, 5);
    assert_int_equal(tree.m_top->lc->rc->key, 10);

    assert_int_equal(tree.m_top->lc->lc->lc->key, 4);

    assert_int_equal(tree.m_top->lc->rc->lc->key, 7);
    assert_int_equal(tree.m_top->lc->rc->rc->key, 11);

    assert_int_equal(tree.m_top->rc->rc->key, 16);

    test_free(sbuf);
    test_free(objs);

}

#if 0
static void
test_random_sequence(void **state)
{
    (void)state;
    struct avl_tree tree = AVL_TREE_INIT;

    for (int i = 0; i < 100000; ++i) {
        void *e = test_malloc(1);
        int const n = randnum(1000000);
        int const rc = avl_add(&tree, e, n);
        if (rc != 0) {
            test_free(e);
            e = avl_rem(&tree, n);
            assert_non_null(e);
            test_free(e);
        }
    }

    int const tree_health = verify_tree(&tree);
    assert_int_equal(tree_health, 0);
    assert_true(1.0*avl_height(&tree) < 1.44*log2(tree.size));

    while (tree.size > 0) {
        void *e = avl_rem(&tree, tree.top->key);
        test_free(e);
    }
}

static void
test_random_sequence_long(void **state)
{
    (void)state;
    struct avl_tree tree = AVL_TREE_INIT;

    for (int i = 0; i < 10000000; ++i) {
        void *e = test_malloc(1);
        int const n = randnum(2000000000);
        int const rc = avl_add(&tree, e, n);
        if (rc != 0) {
            test_free(e);
            e = avl_rem(&tree, n);
            assert_non_null(e);
            test_free(e);
        }
    }

    //printf("Tree was a maximum size of %d\n", tree.size);
    //printf("Tree had a height of %d\n", avl_height(&tree));

    int const tree_health = verify_tree(&tree);
    assert_int_equal(tree_health, 0);
    assert_true(1.0*avl_height(&tree) < 1.44*log2(tree.size));

    while (tree.size > 0) {
        void *e = avl_rem(&tree, tree.top->key);
        test_free(e);
    }
}

// Test to ensure long sequences of sequential numbers doesn't
// cause any issues.
static void
test_degenerate_tree_asc(void **state)
{
    (void)state;
    struct avl_tree tree = AVL_TREE_INIT;

    for (int i = 0; i < 100000; ++i) {
        void *e = test_malloc(1);
        int const rc = avl_add(&tree, e, i);
        assert_return_code(rc, 0);
    }

    //printf("Tree was a maximum size of %d\n", tree.size);
    //printf("Tree had a height of %d\n", avl_height(&tree));
    assert_true(1.0*avl_height(&tree) < 1.44*log2(tree.size));

    int const tree_health = verify_tree(&tree);
    assert_int_equal(tree_health, 0);

    while (tree.size > 0) {
        void *e = avl_rem(&tree, tree.top->key);
        test_free(e);
    }
}

// Test to ensure filling and empyting the tree multiple times
// doesn't cause any weird behavior.
static void
test_random_sequence_repeated(void **state)
{
    (void)state;
    struct avl_tree tree = AVL_TREE_INIT;

    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 100; ++j) {
            void *e = test_malloc(1);
            int const n = randnum(1000000);
            int const rc = avl_add(&tree, e, n);
            if (rc != 0) {
                test_free(e);
                e = avl_rem(&tree, n);
                assert_non_null(e);
                test_free(e);
            }
        }
        int const tree_health = verify_tree(&tree);
        assert_true(1.0*avl_height(&tree) < 1.44*log2(tree.size));
        assert_int_equal(tree_health, 0);

        while (tree.size > 0) {
            void *e = avl_rem(&tree, tree.top->key);
            test_free(e);
        }
    }

}

static void
test_iterator_basic_forward(void **state)
{
    (void)state;

    struct avl_tree tree = AVL_TREE_INIT;
    struct avl_it it;

    avl_add(&tree, NULL, 1);
    avl_add(&tree, NULL, 2);
    avl_add(&tree, NULL, 3);
    avl_add(&tree, NULL, 4);
    avl_add(&tree, NULL, 5);
    avl_add(&tree, NULL, 6);
    avl_add(&tree, NULL, 7);
    avl_add(&tree, NULL, 8);
    avl_add(&tree, NULL, 9);

    {
        avl_it_start(&it, &tree, 0);

        void *e = NULL;
        int key = 0;
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 1);
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 2);
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 3);
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 4);
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 5);
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 6);
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 7);
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 8);
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 9);

        int const rc = avl_it_next(&it, &key, &e);
        assert_int_equal(rc, 1);
    }

    while (tree.size > 0) {
        avl_rem(&tree, tree.top->key);
    }
}

static void
test_iterator_basic_backward(void **state)
{
    (void)state;

    struct avl_tree tree = AVL_TREE_INIT;
    struct avl_it it;


    avl_add(&tree, NULL, 1);
    avl_add(&tree, NULL, 2);
    avl_add(&tree, NULL, 3);
    avl_add(&tree, NULL, 4);
    avl_add(&tree, NULL, 5);
    avl_add(&tree, NULL, 6);
    avl_add(&tree, NULL, 7);
    avl_add(&tree, NULL, 8);
    avl_add(&tree, NULL, 9);

    {
        avl_it_start(&it, &tree, 1);

        void *e = NULL;
        int key = 0;
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 9);
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 8);
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 7);
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 6);
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 5);
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 4);
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 3);
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 2);
        avl_it_next(&it, &key, &e);
        assert_int_equal(key, 1);

        int const rc = avl_it_next(&it, &key, &e);
        assert_int_equal(rc, 1);
    }

    while (tree.size > 0) {
        avl_rem(&tree, tree.top->key);
    }
}

/*
 * Test to ensure that the iterator instance is properly
 * invalidated when the tree structure is mutated.
 *
 */
static void
test_iterator_mutated(void **state) {
    (void)state;

    struct avl_tree tree = AVL_TREE_INIT;
    struct avl_it it;

    avl_add(&tree, NULL, 1);
    avl_add(&tree, NULL, 2);
    avl_add(&tree, NULL, 3);

    avl_it_start(&it, &tree, 0);

    int key = 0;
    void *e = NULL;
    avl_it_next(&it, &key, &e);
    assert_int_equal(key, 1);

    avl_add(&tree, NULL, 5);

    int const rc = avl_it_next(&it, &key, &e);
    assert_int_equal(rc, -1);

    while (tree.size > 0) {
        avl_rem(&tree, tree.top->key);
    }
}


static void
test_min_and_max_elems(void **state) {

    (void)state;
    struct avl_tree tree = AVL_TREE_INIT;

    for (int i = -5; i < 10; ++i) {
        int const rc = avl_add(&tree, NULL, i);
        assert_return_code(rc, 0);
    }

    assert_int_equal(tree.size, 15);
    int key = 0;
    assert_return_code(avl_min_key(&tree, &key), 0);
    assert_int_equal(key, -5);
    assert_return_code(avl_max_key(&tree, &key), 0);
    assert_int_equal(key, 9);

    while (tree.size > 0) {
        avl_rem(&tree, tree.top->key);
    }
}

static void
test_map_functionality(void **state)
{
    (void)state;
    // This test is to ensure that the tree works as a map.
    // First, a number of key-value pairs are generated randomly,
    // then inserted into the tree. Then each key is used to look up the
    // associated value in the tree, which is then checked against the
    // expected value.

    struct key_value_pair {
        int key;
        void *value;
    };
    int const test_extent = 10000;
    struct key_value_pair *items = test_malloc(sizeof(*items) * test_extent);

    for (int i = 0; i < test_extent; ++i) {
        items[i].key = randnum(1000000);
        items[i].value = (void *)randuptr();
    }

    struct avl_tree tree = AVL_TREE_INIT;

    for (int i = 0; i < test_extent; ++i) {
        int const rc = avl_add(&tree, items[i].value, items[i].key);

        // If the key is a duplicate, just overwrite the value in the lookup table.
        if (rc == -1) {
            items[i].value = avl_get(&tree, items[i].key);
        }
    }

    for (int i = 0; i < test_extent; ++i) {
        void *const val = avl_get(&tree, items[i].key);
        assert_ptr_equal(val, items[i].value);
    }

    while (tree.size > 0) {
        avl_rem(&tree, tree.top->key);
    }

    test_free(items);
}

#endif

int main(void) {

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_zero_size_tree),
        cmocka_unit_test(test_small_stack),
        cmocka_unit_test(test_basic_functionality),
        cmocka_unit_test(test_basic_right_right_rotate),
        cmocka_unit_test(test_basic_multiple_nodes),
        cmocka_unit_test(test_known_sequence),
        cmocka_unit_test(test_left_right_rotation),
#if 0
        cmocka_unit_test(test_random_sequence),
        cmocka_unit_test(test_degenerate_tree_asc),
        cmocka_unit_test(test_random_sequence_repeated),
        cmocka_unit_test(test_iterator_basic_forward),
        cmocka_unit_test(test_iterator_basic_backward),
        cmocka_unit_test(test_iterator_mutated),
        //cmocka_unit_test(test_random_sequence_long),
        cmocka_unit_test(test_min_and_max_elems),
        cmocka_unit_test(test_map_functionality),
#endif
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
