
cmake_minimum_required (VERSION 3.11)

add_library( ast Ast.cpp AstIO.cpp )

target_link_libraries( ast util )

