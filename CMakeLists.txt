
project (LLLM)
cmake_minimum_required (VERSION 3.11)

#set( CMAKE_VERBOSE_MAKEFILE ON )

#### ADD SUBDIRS

add_compile_options(-rdynamic)

## Boehm-Demer-Weiser GC
add_subdirectory(lib/bdwgc)
include_directories(lib/bdwgc/include)

## libjit
add_subdirectory(lib/libjit)

## our code
enable_testing()
include_directories(include)
add_subdirectory(src)
