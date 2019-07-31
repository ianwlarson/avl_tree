
cmake_minimum_required(VERSION 3.0.2)

project(avltree C)

set(CMAKE_C_STANDARD 11)

find_package(cmocka)

# Normal version of the library, no warnings or anything.
add_library(avltree SHARED avl.c avl_it.c)

# Version with debug symbols, warnings, no optimizations, debug symbols
add_library(avltree_test SHARED avl.c avl_it.c)
target_compile_options(avltree_test PRIVATE -g -O0 --coverage -Wall -Wextra -Wpedantic)
target_link_libraries(avltree_test gcov)

enable_testing()

add_executable(avltest avltest.c)
target_link_libraries(avltest avltree_test cmocka m)
target_compile_options(avltest PRIVATE -O2 -Wall -Wextra -Wpedantic)
add_test(NAME avltest COMMAND avltest)


