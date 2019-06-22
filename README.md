
# AVL Tree

An AVL tree is a flavour of _Rank-Balanced Binary Search Trees_.
It is more rigidly balanced than a Red-Black tree, but indeed every AVL tree may be
coloured red-black.

An AVL tree has an advantage over a Red-Black tree for use cases where insertions
and deletions are infrequent relative to number of element access.

## Implementation

This particular tree is implemented without using recursion or parent pointers in
the node structures. In order to do this, there is a moderate (~400 bytes) stack usage requirement 
of examining the tree.


