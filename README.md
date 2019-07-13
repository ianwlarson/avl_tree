
# AVL Tree

An AVL tree is a flavour of _Rank-Balanced Binary Search Trees_.
It is more rigidly balanced than a Red-Black tree, but indeed every AVL tree may be
coloured red-black.

An AVL tree has an advantage over a Red-Black tree for use cases where insertions
and deletions are infrequent relative to number of element access.

## Implementation

### Max size
This implementation supports up to (2^31 - 1) elements in the worst case, due to the way the tree is traversed. (It supports a max tree height of 45).

### Details
This particular tree is implemented without using recursion or parent pointers in the node structures. In order to do this, the tree structure maintains a stack with a size of 45 pointers. This gives the base tree structure an additional 360-bytes of size on 64-bit platforms, but reduces the per-node cost by 8-bytes, and reduces the runtime stack usage drastically.


