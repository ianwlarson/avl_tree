
cmake_minimum_required(VERSION 3.0.2)

project(avltree C)

set(CMAKE_C_STANDARD 11)

find_package(cmocka)

# Normal version of the library
add_library(avltree STATIC avl.c)
target_compile_options(avltree PRIVATE -Wall -Wextra -Wpedantic)

# Version with wrapped malloc/free
add_library(avltree_test SHARED avl.c)
target_compile_options(avltree_test PRIVATE -g -Wall -Wextra -Wpedantic)
target_link_libraries(avltree_test cmocka)

enable_testing()
add_executable(avltest avltest.c)
target_link_libraries(avltest avltree_test cmocka)
target_compile_options(avltest PRIVATE -g -Og -Wall -Wextra -Wpedantic)
add_test(NAME avltest COMMAND avltest)
