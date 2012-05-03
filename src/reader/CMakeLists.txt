
add_library( reader reader.cpp Tokenizer.cpp SourceLocation.cpp Sexpr.cpp SexprIO.cpp )

target_link_libraries( reader util )

