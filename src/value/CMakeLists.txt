
cmake_minimum_required(VERSION 3.11)

add_library( value Value.cpp ValueIO.cpp )

target_link_libraries( value util ast )
