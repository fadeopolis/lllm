
cmake_minimum_required(VERSION 3.11)

add_library( sexpr Sexpr.cpp SexprIO.cpp )

target_link_libraries( sexpr util )

