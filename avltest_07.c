
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
        1, 2, 3,
    };

    int removes[] = {
        2
    };
    int const n_inputs = sizeof(inputs)/sizeof(*inputs);
    int const n_removes = sizeof(removes)/sizeof(*removes);

    my_t *objs = malloc(sizeof(*objs) * n_inputs);

    for (int i = 0; i < n_inputs; ++i) {
        objs[i].my_key = inputs[i];
        avl_my_add(tree, &objs[i]);
    }

    for (int i = 0; i < n_removes; ++i) {
        avl_my_rem(tree, removes[i]);
    }

    {
        my_t *one = nd2t(tree->m_top);
        assert(KEY(one) == 1);
        assert(BF(one) == 1);
        assert(leftc(one) == NULL);

        my_t *three = rightc(one);
        assert(KEY(three) == 3);
        assert(BF(three) == 0);
        assert(leftc(three) == NULL && rightc(three) == NULL);
    }

    avl_my_rem(tree, 1);

    {
        my_t *three = nd2t(tree->m_top);
        assert(KEY(three) == 3);
        assert(BF(three) == 0);
        assert(leftc(three) == NULL && rightc(three) == NULL);
    }

    return 0;
}
