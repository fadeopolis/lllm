
project (LLLM)
cmake_minimum_required (VERSION 2.8.5)

## CONFIGURE COMPILER & LINKER
set( CMAKE_C_COMPILER   clang )
set( CMAKE_CXX_COMPILER clang++ )
set( CMAKE_SHARED_LINKER_FLAGS -rdynamic )
add_definitions(-std=gnu++11)

## ADD SUBDIRS
include_directories( include )
add_subdirectory( src )

