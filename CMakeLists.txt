
cmake_minimum_required(VERSION 3.0.2)

project(avltree C)

set(CMAKE_C_STANDARD 11)

find_package(cmocka)

# Normal version of the library
add_library(avltree SHARED avl.c avl_it.c)
target_compile_options(avltree PRIVATE -Wall -Wextra -Wpedantic)

# Version with debug symbols
add_library(avltree_test SHARED avl.c avl_it.c)
target_compile_options(avltree_test PRIVATE --coverage -g -Wall -Wextra -Wpedantic)
target_link_libraries(avltree_test gcov)

enable_testing()
add_executable(avltest avltest.c)
target_link_libraries(avltest avltree_test cmocka gcov)
target_compile_options(avltest PRIVATE --coverage -g -Wall -Wextra -Wpedantic)
add_test(NAME avltest COMMAND avltest)
