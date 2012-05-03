
add_library( analyzer analyzer.cpp Ast.cpp AstIO.cpp Scope.cpp )

target_link_libraries( analyzer util reader )

