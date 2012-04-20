
add_subdirectory( fail )
add_subdirectory( value )
add_subdirectory( reader )
add_subdirectory( eval )

add_executable( lllm lllm.cpp )
target_link_libraries(lllm value reader eval fail)
