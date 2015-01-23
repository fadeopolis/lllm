
project (LLLM)
cmake_minimum_required (VERSION 2.8.5)

#set( CMAKE_VERBOSE_MAKEFILE ON )

#### ADD SUBDIRS


## Boehm-Demer-Weiser GC
add_subdirectory(lib/bdwgc)
include_directories(lib/bdwgc/include)

## libjit
add_subdirectory(lib/libjit)
include_directories(lib/libjit/include)
link_directories(lib/libjit/jit/.libs)

##p99
include_directories(lib/p99)

include_directories(include)
add_subdirectory(src)
