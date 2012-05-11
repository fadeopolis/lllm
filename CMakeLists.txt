
project (LLLM)
cmake_minimum_required (VERSION 2.8.5)

#set( CMAKE_VERBOSE_MAKEFILE ON )

#### CONFIGURE COMPILER & LINKER
set( CMAKE_CXX_COMPILER        clang++ )
#set( CMAKE_CXX_LINK_EXECUTABLE clang )
set( CMAKE_SHARED_LINKER_FLAGS "-g -rdynamic" )

add_definitions( -std=gnu++11 )
add_definitions( -g )
add_definitions( -Wall )
## LLVM FLAGS
#add_definitions( -DNDEBUG )
#add_definitions( -D_GNU_SOURCE )
#add_definitions( -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS )
#add_definitions( -fomit-frame-pointer -fno-exceptions -fPIC -Woverloaded-virtual -Wcast-qual )

#### ADD SUBDIRS
#link_directories( /home/fader/Desktop/llvm/Debug+Asserts/lib )
#link_directories( /usr/lib/llvm-3.0/lib )

link_directories( lib )
include_directories( include lib )
add_subdirectory( src )

