
set(CMAKE_CXX_COMPILER        clang++)
# set(CMAKE_CXX_LINK_EXECUTABLE clang)
set(CMAKE_SHARED_LINKER_FLAGS "-g -rdynamic")

add_definitions( -std=gnu++11 )
add_definitions( -g )
add_definitions( -Wall )

add_subdirectory( util  )
add_subdirectory( sexpr )
add_subdirectory( ast   )
add_subdirectory( value )

add_library( lllm lllm.cpp Reader.cpp Analyzer.cpp Evaluator.cpp EscapeAnalyzer.cpp Jit.cpp Builtins.cpp GlobalScope.cpp )
add_library( jit  STATIC IMPORTED )
set_property( TARGET jit PROPERTY IMPORTED_LOCATION /home/fader/Desktop/libjit.a )

target_link_libraries(lllm
	## lllm libs
	sexpr
	ast
	value
	# system libs
	gc-lib
	${LIBJIT}
	pthread
	ffi
	dl
	m
)

add_executable(repl repl.cpp)
target_link_libraries(repl lllm gc-lib ${LIBJIT})

## testing
add_executable( test_1_reader   test_1_reader.cpp   )
add_executable( test_2_analyzer test_2_analyzer.cpp )
add_executable( test_3_eval     test_3_eval.cpp     )
add_executable( test_4_jit      test_4_jit.cpp )

target_link_libraries( test_1_reader   lllm )
target_link_libraries( test_2_analyzer lllm )
target_link_libraries( test_3_eval     lllm )
target_link_libraries( test_4_jit      lllm )


