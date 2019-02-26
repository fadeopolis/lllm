
cmake_minimum_required(VERSION 3.11)

set(CMAKE_SHARED_LINKER_FLAGS "-g -rdynamic")

add_definitions( -std=gnu++11 )
add_definitions( -g )
add_definitions( -Wall )

add_subdirectory( util  )
add_subdirectory( sexpr )
add_subdirectory( ast   )
add_subdirectory( value )

add_library( lllm lllm.cpp Reader.cpp Analyzer.cpp Evaluator.cpp EscapeAnalyzer.cpp Jit.cpp Builtins.cpp GlobalScope.cpp )

target_link_libraries(lllm
	## lllm libs
	sexpr
	ast
	value
	# system libs
	gc-lib
	libjit
	pthread
	ffi
	dl
	m
)

add_executable(repl repl.cpp)
target_link_libraries(repl lllm)

## testing
add_executable( test_1_reader   test_1_reader.cpp   )
add_executable( test_2_analyzer test_2_analyzer.cpp )
add_executable( test_3_eval     test_3_eval.cpp     )
add_executable( test_4_jit      test_4_jit.cpp )

target_link_libraries( test_1_reader   lllm )
target_link_libraries( test_2_analyzer lllm )
target_link_libraries( test_3_eval     lllm )
target_link_libraries( test_4_jit      lllm )

add_test(NAME test_1_reader   COMMAND test_1_reader)
add_test(NAME test_2_analyzer COMMAND test_2_analyzer)
add_test(NAME test_3_eval     COMMAND test_3_eval)
add_test(NAME test_4_jit      COMMAND test_4_jit)

install(
	TARGETS repl
	RUNTIME DESTINATION bin
)
