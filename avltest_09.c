
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
        5, 2, 8, 1, 3, 7, 10, 4, 6, 9, 11, 12
    };
    int const n_objs = sizeof(insertions)/sizeof(*insertions);
    my_t *objs = malloc(sizeof(*objs) * n_objs);

    for (int i = 0; i < n_objs; ++i) {
        objs[i].my_key = insertions[i];
        avl_my_add(tree, &objs[i]);
    }

    {
        my_t *five = TOP(tree);
        assert(KEY(five) == 5);
        assert(BF(five) == 1);

        my_t *two = leftc(five);
        assert(KEY(two) == 2);
        assert(BF(two) == 1);

        my_t *eight = rightc(five);
        assert(KEY(eight) == 8);
        assert(BF(eight) == 1);

        my_t *ten = rightc(eight);
        assert(KEY(ten) == 10);
        assert(BF(ten) == 1);
    }

    avl_my_rem(tree, 1);
    {
        my_t *eight = TOP(tree);
        assert(KEY(eight) == 8);
        assert(BF(eight) == 0);

        my_t *five = leftc(eight);
        assert(KEY(five) == 5);
        assert(BF(five) == 0);

        my_t *three = leftc(five);
        assert(KEY(three) == 3);
        assert(BF(three) == 0);

        my_t *seven = rightc(five);
        assert(KEY(seven) == 7);
        assert(BF(seven) == -1);
        assert(rightc(seven) == NULL);

        my_t *six = leftc(seven);
        assert(KEY(six) == 6);
        assert(BF(six) == 0);
        assert(leftc(six) == NULL && rightc(six) == NULL);

        my_t *two = leftc(three);
        assert(KEY(two) == 2);
        assert(BF(two) == 0);
        assert(leftc(two) == NULL && rightc(two) == NULL);

        my_t *four = rightc(three);
        assert(KEY(four) == 4);
        assert(BF(four) == 0);
        assert(leftc(four) == NULL && rightc(four) == NULL);

        my_t *ten = rightc(eight);
        assert(KEY(ten) == 10);
        assert(BF(ten) == 1);

        my_t *eleven = rightc(ten);
        assert(KEY(eleven) == 11);
        assert(BF(eleven) == 1);
        assert(leftc(eleven) == NULL);

        my_t *twelve = rightc(eleven);
        assert(KEY(twelve) == 12);
        assert(BF(twelve) == 0);
        assert(leftc(twelve) == NULL && rightc(twelve) == NULL);

        my_t *nine = leftc(ten);
        assert(KEY(nine) == 9);
        assert(BF(nine) == 0);
        assert(leftc(nine) == NULL && rightc(nine) == NULL);
    }


    return 0;
}
