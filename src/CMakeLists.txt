
add_subdirectory( util )
add_subdirectory( reader )
#add_subdirectory( value )
#add_subdirectory( eval )

add_executable( lllm lllm.cpp )
target_link_libraries(lllm reader)
