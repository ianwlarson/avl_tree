
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

    my_t *objs = malloc(sizeof(*objs) * 4);
    // This series of insertions and deletions causes a simple rotation.
    int insertions[] = {
        2, 4, 6, 3,
    };
    int deletions[] = {
        6,
    };
    for (int i = 0; i < sizeof(insertions)/sizeof(*insertions); ++i) {
        objs[i].my_key = insertions[i];
        avl_my_add(tree, &objs[i]);
    }
    for (int i = 0; i < sizeof(deletions)/sizeof(*deletions); ++i) {
        avl_my_rem(tree, deletions[i]);
    }

    {
        my_t *three = nd2t(tree->m_top);
        assert(KEY(three) == 3);
        assert(BF(three) == 0);

        my_t *two = leftc(three);
        assert(KEY(two) == 2);
        assert(BF(two) == 0);
        assert(leftc(two) == NULL && rightc(two) == NULL);

        my_t *four = rightc(three);
        assert(KEY(four) == 4);
        assert(BF(four) == 0);
        assert(leftc(four) == NULL && rightc(four) == NULL);

    }

    return 0;
}
