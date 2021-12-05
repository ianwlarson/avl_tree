
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

    int inputs[] = {
        74, 0, 71, 47, 76, 82, 25, 50, 68, 17, 29, 98, 48, 14, 97, 28, 43, 89, 58, 3, 13
    };

    int removes[] = {
        98, 58, 29, 47, 3, 50, 68
    };

    int const n_inputs = sizeof(inputs)/sizeof(*inputs);
    int const n_removes = sizeof(removes)/sizeof(*removes);

    my_t *objs = malloc(sizeof(*objs) * n_inputs);

    for (int i = 0; i < n_inputs; ++i) {
        objs[i].my_key = inputs[i];
        avl_my_add(tree, &objs[i]);
    }

    {
        my_t *five_zero = TOP(tree);
        assert(KEY(five_zero) == 50);
        assert(BF(five_zero) == 0);

        my_t *two_five = leftc(five_zero);
        assert(KEY(two_five) == 25);
        assert(BF(two_five) == 0);

        my_t *seven_six = rightc(five_zero);
        assert(KEY(seven_six) == 76);
        assert(BF(seven_six) == 0);
    }

    avl_my_rem(tree, 98);
    {
        my_t *five_zero = TOP(tree);
        assert(KEY(five_zero) == 50);
        assert(BF(five_zero) == 0);

        my_t *two_five = leftc(five_zero);
        assert(KEY(two_five) == 25);
        assert(BF(two_five) == 0);

        my_t *seven_six = rightc(five_zero);
        assert(KEY(seven_six) == 76);
        assert(BF(seven_six) == -1);
    }

    avl_my_rem(tree, 58);
    {
        my_t *five_zero = TOP(tree);
        assert(KEY(five_zero) == 50);
        assert(BF(five_zero) == -1);

        my_t *two_five = leftc(five_zero);
        assert(KEY(two_five) == 25);
        assert(BF(two_five) == 0);

        my_t *seven_six = rightc(five_zero);
        assert(KEY(seven_six) == 76);
        assert(BF(seven_six) == 0);
    }

    avl_my_rem(tree, 29);
    {
        my_t *five_zero = TOP(tree);
        assert(KEY(five_zero) == 50);
        assert(BF(five_zero) == -1);

        my_t *two_five = leftc(five_zero);
        assert(KEY(two_five) == 25);
        assert(BF(two_five) == 0);

        my_t *seven_six = rightc(five_zero);
        assert(KEY(seven_six) == 76);
        assert(BF(seven_six) == 0);
    }

    avl_my_rem(tree, 47);
    {
        my_t *five_zero = TOP(tree);
        assert(KEY(five_zero) == 50);
        assert(BF(five_zero) == -1);

        my_t *two_five = leftc(five_zero);
        assert(KEY(two_five) == 25);
        assert(BF(two_five) == -1);

        my_t *seven_six = rightc(five_zero);
        assert(KEY(seven_six) == 76);
        assert(BF(seven_six) == 0);
    }

    avl_my_rem(tree, 3);
    avl_my_rem(tree, 50);
    avl_my_rem(tree, 68);

    {
        my_t *four_eight = nd2t(tree->m_top);
        assert(KEY(four_eight) == 48);
        assert(BF(four_eight) == -1);

        my_t *two_five = leftc(four_eight);
        assert(KEY(two_five) == 25);
        assert(BF(two_five) == -1);

        my_t *seven_six = rightc(four_eight);
        assert(KEY(seven_six) == 76);
        assert(BF(seven_six) == 0);
    }

    return 0;
}
