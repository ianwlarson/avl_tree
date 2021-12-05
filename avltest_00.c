
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

    // This series of insertions causes a simple rotation
    int insertions[] = {
        1, 2, 3,
    };
    int const n_objs = sizeof(insertions)/sizeof(*insertions);
    my_t *objs = malloc(sizeof(*objs) * n_objs);

    for (int i = 0; i < n_objs; ++i) {
        objs[i].my_key = insertions[i];
        avl_my_add(tree, &objs[i]);
    }

    {
        my_t *two = TOP(tree);
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
    }

    return 0;
}
