
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

    int insertions[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9
    };
    int const n_objs = sizeof(insertions)/sizeof(*insertions);
    my_t *objs = malloc(sizeof(*objs) * n_objs);

    for (int i = 0; i < n_objs; ++i) {
        objs[i].my_key = insertions[i];
        avl_my_add(tree, &objs[i]);
    }

    {
        my_t *three = nd2t(tree->m_top);
        assert(KEY(three) == 3);
        assert(BF(three) == 1);

        my_t *one = leftc(three);
        assert(KEY(one) == 1);
        assert(BF(one) == 0);

        my_t *zero = leftc(one);
        assert(KEY(zero) == 0);
        assert(BF(zero) == 0);
        assert(leftc(zero) == NULL && rightc(zero) == NULL);

        my_t *two = rightc(one);
        assert(KEY(two) == 2);
        assert(BF(two) == 0);
        assert(leftc(two) == NULL && rightc(two) == NULL);

        my_t *seven = rightc(three);
        assert(KEY(seven) == 7);
        assert(BF(seven) == 0);

        my_t *five = leftc(seven);
        assert(KEY(five) == 5);
        assert(BF(five) == 0);

        my_t *four = leftc(five);
        assert(KEY(four) == 4);
        assert(BF(four) == 0);
        assert(leftc(four) == NULL && rightc(four) == NULL);

        my_t *six = rightc(five);
        assert(KEY(six) == 6);
        assert(BF(six) == 0);
        assert(leftc(six) == NULL && rightc(six) == NULL);

        my_t *eight = rightc(seven);
        assert(KEY(eight) == 8);
        assert(BF(eight) == 1);
        assert(leftc(eight) == NULL);

        my_t *nine = rightc(eight);
        assert(KEY(nine) == 9);
        assert(BF(nine) == 0);
        assert(leftc(nine) == NULL && rightc(nine) == NULL);
    }
    avl_my_rem(tree, 7);
    {
        my_t *three = nd2t(tree->m_top);
        assert(KEY(three) == 3);
        assert(BF(three) == 1);

        my_t *six = rightc(three);
        assert(KEY(six) == 6);
        assert(BF(six) == 0);

        my_t *five = leftc(six);
        assert(KEY(five) == 5);
        assert(BF(five) == -1);
        assert(rightc(five) == NULL);

        my_t *four = leftc(five);
        assert(KEY(four) == 4);
        assert(BF(four) == 0);
        assert(leftc(four) == NULL && rightc(four) == NULL);

        my_t *eight = rightc(six);
        assert(KEY(eight) == 8);
        assert(BF(eight) == 1);
        assert(leftc(eight) == NULL);

        my_t *nine = rightc(eight);
        assert(KEY(nine) == 9);
        assert(BF(nine) == 0);
        assert(leftc(nine) == NULL && rightc(nine) == NULL);
    }
    avl_my_rem(tree, 2);
    avl_my_rem(tree, 0);
    {
        my_t *six = nd2t(tree->m_top);
        assert(KEY(six) == 6);
        assert(BF(six) == -1);

        my_t *three = leftc(six);
        assert(KEY(three) == 3);
        assert(BF(three) == 1);

        my_t *five = rightc(three);
        assert(KEY(five) == 5);
        assert(BF(five) == -1);
        assert(rightc(five) == NULL);

        my_t *eight = rightc(six);
        assert(KEY(eight) == 8);
        assert(BF(eight) == 1);
        assert(leftc(eight) == NULL);

        my_t *nine = rightc(eight);
        assert(KEY(nine) == 9);
        assert(BF(nine) == 0);
        assert(leftc(nine) == NULL && rightc(nine) == NULL);

        my_t *four = leftc(five);
        assert(KEY(four) == 4);
        assert(BF(four) == 0);
        assert(leftc(four) == NULL && rightc(four) == NULL);
    }
    avl_my_rem(tree, 9);
    {
        my_t *five = nd2t(tree->m_top);
        assert(KEY(five) == 5);
        assert(BF(five) == 0);

        my_t *three = leftc(five);
        assert(KEY(three) == 3);
        assert(BF(three) == 0);

        my_t *one = leftc(three);
        assert(KEY(one) == 1);
        assert(BF(one) == 0);
        assert(leftc(one) == NULL && rightc(one) == NULL);

        my_t *four = rightc(three);
        assert(KEY(four) == 4);
        assert(BF(four) == 0);
        assert(leftc(four) == NULL && rightc(four) == NULL);

        my_t *six = rightc(five);
        assert(KEY(six) == 6);
        assert(BF(six) == 1);
        assert(leftc(six) == NULL);

        my_t *eight = rightc(six);
        assert(KEY(eight) == 8);
        assert(BF(eight) == 0);
        assert(leftc(eight) == NULL && rightc(eight) == NULL);
    }
    avl_my_rem(tree, 6);
    {
        my_t *five = nd2t(tree->m_top);
        assert(KEY(five) == 5);
        assert(BF(five) == -1);

        my_t *eight = rightc(five);
        assert(KEY(eight) == 8);
        assert(BF(eight) == 0);
        assert(leftc(eight) == NULL && rightc(eight) == NULL);

        my_t *three = leftc(five);
        assert(KEY(three) == 3);
        assert(BF(three) == 0);

        my_t *four = rightc(three);
        assert(KEY(four) == 4);
        assert(BF(four) == 0);
        assert(leftc(four) == NULL && rightc(four) == NULL);
    }
    avl_my_rem(tree, 8);
    {
        my_t *three = nd2t(tree->m_top);
        assert(KEY(three) == 3);
        assert(BF(three) == 1);

        my_t *one = leftc(three);
        assert(KEY(one) == 1);
        assert(BF(one) == 0);
        assert(leftc(one) == NULL && rightc(one) == NULL);

        my_t *five = rightc(three);
        assert(KEY(five) == 5);
        assert(BF(five) == -1);
        assert(rightc(five) == NULL);

        my_t *four = leftc(five);
        assert(KEY(four) == 4);
        assert(BF(four) == 0);
        assert(leftc(four) == NULL && rightc(four) == NULL);
    }

    return 0;
}
