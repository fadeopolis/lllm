
add_library( reader reader.cpp SourceLocation.cpp Sexpr.cpp SexprIO.cpp )

target_link_libraries( reader util )

