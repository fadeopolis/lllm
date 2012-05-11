
add_library( analyzer analyzer.cpp Ast.cpp AstIO.cpp Scope.cpp TypeSet.cpp )

target_link_libraries( analyzer util reader )

