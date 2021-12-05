
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

    my_t *objs = malloc(sizeof(*objs) * 6);
    int order[] = {
        9, 6, 3, 0, 5, 4
    };
    for (int i = 0; i < sizeof(order)/sizeof(*order); ++i) {
        objs[i].my_key = order[i];
        avl_my_add(tree, &objs[i]);
    }

    {
        my_t *five = nd2t(tree->m_top);
        assert(KEY(five) == 5);
        assert(BF(five) == 0);

        my_t *three = leftc(five);
        assert(KEY(three) == 3);
        assert(BF(three) == 0);
        my_t *zero = leftc(three);
        assert(KEY(zero) == 0);
        assert(BF(zero) == 0);
        assert(leftc(zero) == NULL && rightc(zero) == NULL);

        my_t *four = rightc(three);
        assert(KEY(four) == 4);
        assert(BF(four) == 0);
        assert(leftc(four) == NULL && rightc(four) == NULL);

        my_t *six = rightc(five);
        assert(KEY(six) == 6);
        assert(BF(six) == 1);
        assert(leftc(six) == NULL);

        my_t *nine = rightc(six);
        assert(KEY(nine) == 9);
        assert(BF(nine) == 0);
        assert(leftc(nine) == NULL && rightc(nine) == NULL);
    }

    return 0;
}
