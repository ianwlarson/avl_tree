
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

    // This series of insertions causes a Double-Left rotate.
    int order[] = {
        0, 3, 6, 9, 4, 5
    };
    for (int i = 0; i < sizeof(order)/sizeof(*order); ++i) {
        objs[i].my_key = order[i];
        avl_my_add(tree, &objs[i]);
    }

    {
        my_t *four = nd2t(tree->m_top);
        assert(KEY(four) == 4);
        assert(BF(four) == 0);

        my_t *three = leftc(four);
        assert(KEY(three) == 3);
        assert(BF(three) == -1);
        assert(rightc(three) == NULL);

        my_t *zero = leftc(three);
        assert(KEY(zero) == 0);
        assert(BF(zero) == 0);
        assert(leftc(zero) == NULL && rightc(zero) == NULL);

        my_t *six = rightc(four);
        assert(KEY(six) == 6);
        assert(BF(six) == 0);

        my_t *five = leftc(six);
        assert(KEY(five) == 5);
        assert(BF(five) == 0);
        assert(leftc(five) == NULL && rightc(five) == NULL);

        my_t *nine = rightc(six);
        assert(KEY(nine) == 9);
        assert(BF(nine) == 0);
        assert(leftc(nine) == NULL && rightc(nine) == NULL);
    }

    return 0;
}
