
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>
#include <time.h>

#include "avl.h"
#include "avl_it.h"

struct avl_node *
create_new_node(void *const elem, int const key)
{
    struct avl_node *n = test_malloc(sizeof(*n));

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
    test_free(node);
}

unsigned long rng_state;
static inline unsigned long
xorshift32(void)
{
    unsigned long x = rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng_state = x;
    return rng_state;
}

static inline int
randnum(void)
{
    int o = xorshift32();
    return o % 1000;
}


/* A test case that does nothing and succeeds. */
static void null_test_success(void **state) {
    (void) state; /* unused */
}

static void
test_basic_functionality(void **state)
{
    (void) state;

    void *const ptr = test_malloc(10);

    struct avl_tree tree = AVL_TREE_INIT;

    // Test that an element can be added
    int const rc = avl_add(&tree, ptr, 1);
    assert_return_code(rc, 0);
    assert_int_equal(tree.size, 1);

    // Test than a non-existing element won't be found
    void *const p1 = avl_get(&tree, 22);
    assert_null(p1);

    // Test that the element can be found
    void *const p2 = avl_get(&tree, 1);
    assert_non_null(p2);
    assert_ptr_equal(p2, ptr);
    assert_int_equal(avl_height(&tree), 1);

    // Test that trying to remove a non-existing
    // element won't do anything
    void *const p3 = avl_rem(&tree, 0);
    assert_null(p3);
    assert_int_equal(tree.size, 1);
    assert_int_equal(avl_height(&tree), 1);

    // Test that an existing element can be removed
    void *const p4 = avl_rem(&tree, 1);
    assert_non_null(p4);
    assert_ptr_equal(p2, p4);
    assert_int_equal(tree.size, 0);
    assert_int_equal(avl_height(&tree), 0);

    test_free(p4);
}

static void
test_zero_size_tree(void **state)
{
    (void)state;

    struct avl_tree tree = AVL_TREE_INIT;

    assert_null(avl_get(&tree, 22));
    assert_null(avl_rem(&tree, 11));
    assert_int_equal(avl_height(&tree), 0);
}

static void
test_basic_right_right_rotate(void **state)
{
    (void)state;

    struct avl_tree tree = AVL_TREE_INIT;
    avl_add(&tree, NULL, 1);
    avl_add(&tree, NULL, 2);
    avl_add(&tree, NULL, 3);

    assert_int_equal(avl_height(&tree), 2);
    assert_int_equal(tree.size, 3);

    avl_rem(&tree, 2);
    avl_rem(&tree, 1);
    avl_rem(&tree, 3);

    assert_int_equal(avl_height(&tree), 0);
    assert_int_equal(tree.size, 0);
}

static void
test_basic_multiple_nodes(void **state)
{
    (void)state;

    struct avl_tree tree = AVL_TREE_INIT;

    /* This should produce the following tree
    //                 3 
    //                /  \
    //               /    \
    //              1      7
    //             / \    / \
    //            0   2  5   8
    //                  / \   \
    //                 4   6   9
    */

    for (int i = 0; i < 10; ++i) {
        void *const e = test_malloc(10);
        int const rc = avl_add(&tree, e, i);
        assert_return_code(rc, 0);
    }

    assert_int_equal(tree.size, 10);
    assert_int_equal(avl_height(&tree), 4);
    assert_int_equal(tree.top->key, 3);
    assert_int_equal(tree.top->lc->key, 1);
    assert_int_equal(tree.top->rc->key, 7);

    for (int i = 0; i < 10; ++i) {
        void *const e = avl_rem(&tree, i);
        assert_non_null(e);
        test_free(e);
    }

    assert_int_equal(tree.size, 0);
    assert_int_equal(avl_height(&tree), 0);
}

// Testing a sequence based off of a known valid
// implementation on the internet
static void
test_known_sequence(void **state)
{
    (void)state;

    struct avl_tree tree = AVL_TREE_INIT;
    avl_add(&tree, NULL, 1);
    avl_add(&tree, NULL, 9);
    avl_add(&tree, NULL, 2);
    avl_add(&tree, NULL, 8);
    avl_add(&tree, NULL, 3);
    avl_add(&tree, NULL, 7);

    assert_int_equal(tree.top->key, 3);
    assert_int_equal(avl_height(&tree), 3);

    avl_rem(&tree, 3);

    assert_int_equal(tree.top->key, 2);
    assert_int_equal(avl_height(&tree), 3);

    avl_rem(&tree, 2);

    assert_int_equal(tree.top->key, 8);
    assert_int_equal(avl_height(&tree), 3);

    avl_add(&tree, NULL, 5);
    assert_int_equal(tree.top->key, 8);
    assert_int_equal(tree.top->lc->key, 5);
    assert_int_equal(tree.top->lc->lc->key, 1);
    assert_int_equal(tree.top->lc->rc->key, 7);
    assert_int_equal(avl_height(&tree), 3);

    avl_add(&tree, NULL, 10);
    avl_add(&tree, NULL, 6);
    assert_int_equal(tree.top->key, 8);
    assert_int_equal(avl_height(&tree), 4);

    avl_rem(&tree, 8);
    assert_int_equal(tree.top->key, 7);
    assert_int_equal(avl_height(&tree), 3);

    avl_rem(&tree, 9);
    avl_rem(&tree, 10);
    assert_int_equal(tree.top->key, 5);
    assert_int_equal(avl_height(&tree), 3);

    avl_rem(&tree, 1);
    assert_int_equal(tree.top->key, 6);
    assert_int_equal(avl_height(&tree), 2);

    avl_rem(&tree, 5);
    avl_rem(&tree, 6);
    assert_int_equal(tree.top->key, 7);
    assert_int_equal(avl_height(&tree), 1);

    avl_rem(&tree, 7);
    assert_int_equal(tree.size, 0);
}


static void
test_random_sequence(void **state)
{
    (void)state;
    struct avl_tree tree = AVL_TREE_INIT;

    for (int i = 0; i < 100000; ++i) {
        void *e = test_malloc(1);
        int const n = randnum();
        int const rc = avl_add(&tree, e, n);
        if (rc != 0) {
            test_free(e);
            e = avl_rem(&tree, n);
            assert_non_null(e);
            test_free(e);
        }
    }

    while (tree.size > 0) {
        void *e = avl_rem(&tree, tree.top->key);
        test_free(e);
    }
}

static void
test_iterator_basic_forward(void **state)
{
    (void)state;

    struct avl_tree tree = AVL_TREE_INIT; 
    struct avl_itr it;

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
    struct avl_itr it;

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
    struct avl_itr it;

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

int main(void) {

    rng_state = time(NULL);

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(null_test_success),
        cmocka_unit_test(test_zero_size_tree),
        cmocka_unit_test(test_basic_functionality),
        cmocka_unit_test(test_basic_right_right_rotate),
        cmocka_unit_test(test_basic_multiple_nodes),
        cmocka_unit_test(test_known_sequence),
        cmocka_unit_test(test_random_sequence),
        cmocka_unit_test(test_iterator_basic_forward),
        cmocka_unit_test(test_iterator_basic_backward),
        cmocka_unit_test(test_iterator_mutated),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
