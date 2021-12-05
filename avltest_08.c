
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>

#include "avlhelper.h"
#include "avltest.h"

int
main(void)
{
    avl_tree_t t = avl_tree_init();
    avl_tree_t *const tree = &t;

    // This series of insertions and deletions causes a simple rotation.
    int insertions[] = {
        1, 2, 3, 4, 5, 6, 7
    };
    int const n_objs = sizeof(insertions)/sizeof(*insertions);
    my_t *objs = malloc(sizeof(*objs) * n_objs);

    for (int i = 0; i < n_objs; ++i) {
        objs[i].my_key = insertions[i];
        avl_my_add(tree, &objs[i]);
    }

    {
        assert(tree->m_size == 7);
        my_t *four = nd2t(tree->m_top);
        assert(KEY(four) == 4);
        assert(BF(four) == 0);

        my_t *two = leftc(four);
        assert(KEY(two) == 2);
        assert(BF(two) == 0);

        my_t *one = leftc(two);
        assert(KEY(one) == 1);
        assert(BF(one) == 0);
        assert(leftc(one) == NULL && rightc(one) == NULL);

        my_t *three = rightc(two);
        assert(KEY(three) == 3);
        assert(BF(three) == 0);
        assert(leftc(three) == NULL && rightc(three) == NULL);

        my_t *six = rightc(four);
        assert(KEY(six) == 6);
        assert(BF(six) == 0);

        my_t *five = leftc(six);
        assert(KEY(five) == 5);
        assert(BF(five) == 0);
        assert(leftc(five) == NULL && rightc(five) == NULL);

        my_t *seven = rightc(six);
        assert(KEY(seven) == 7);
        assert(BF(seven) == 0);
        assert(leftc(seven) == NULL && rightc(seven) == NULL);
    }

    avl_my_rem(tree, 4);

    {
        my_t *three = nd2t(tree->m_top);
        assert(KEY(three) == 3);
        assert(BF(three) == 0);

        my_t *two = leftc(three);
        assert(KEY(two) == 2);
        assert(BF(two) == -1);
        assert(rightc(two) == NULL);

        my_t *one = leftc(two);
        assert(KEY(one) == 1);
        assert(BF(one) == 0);
        assert(leftc(one) == NULL && rightc(one) == NULL);

        my_t *six = rightc(three);
        assert(KEY(six) == 6);
        assert(BF(six) == 0);

        my_t *five = leftc(six);
        assert(KEY(five) == 5);
        assert(BF(five) == 0);
        assert(leftc(five) == NULL && rightc(five) == NULL);

        my_t *seven = rightc(six);
        assert(KEY(seven) == 7);
        assert(BF(seven) == 0);
        assert(leftc(seven) == NULL && rightc(seven) == NULL);
    }

    avl_my_rem(tree, 3);

    {
        my_t *two = nd2t(tree->m_top);
        assert(KEY(two) == 2);
        assert(BF(two) == 1);

        my_t *one = leftc(two);
        assert(KEY(one) == 1);
        assert(BF(one) == 0);
        assert(leftc(one) == NULL && rightc(one) == NULL);

        my_t *six = rightc(two);
        assert(KEY(six) == 6);
        assert(BF(six) == 0);

        my_t *five = leftc(six);
        assert(KEY(five) == 5);
        assert(BF(five) == 0);
        assert(leftc(five) == NULL && rightc(five) == NULL);

        my_t *seven = rightc(six);
        assert(KEY(seven) == 7);
        assert(BF(seven) == 0);
        assert(leftc(seven) == NULL && rightc(seven) == NULL);
    }

    avl_my_rem(tree, 2);

    {
        my_t *six = nd2t(tree->m_top);
        assert(KEY(six) == 6);
        assert(BF(six) == -1);

        my_t *one = leftc(six);
        assert(KEY(one) == 1);
        assert(BF(one) == 1);
        assert(leftc(one) == NULL);

        my_t *five = rightc(one);
        assert(KEY(five) == 5);
        assert(BF(five) == 0);
        assert(leftc(five) == NULL && rightc(five) == NULL);

        my_t *seven = rightc(six);
        assert(KEY(seven) == 7);
        assert(BF(seven) == 0);
        assert(leftc(seven) == NULL && rightc(seven) == NULL);
    }

    avl_my_rem(tree, 6);

    {
        my_t *five = nd2t(tree->m_top);
        assert(KEY(five) == 5);
        assert(BF(five) == 0);

        my_t *one = leftc(five);
        assert(KEY(one) == 1);
        assert(BF(one) == 0);
        assert(leftc(one) == NULL && rightc(one) == NULL);

        my_t *seven = rightc(five);
        assert(KEY(seven) == 7);
        assert(BF(seven) == 0);
        assert(leftc(seven) == NULL && rightc(seven) == NULL);
    }


    return 0;
}
