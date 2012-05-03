
add_subdirectory( util )
add_subdirectory( reader )
add_subdirectory( analyzer )
add_subdirectory( values )
add_subdirectory( eval )

add_library( vm vm.cpp builtins.cpp )
target_link_libraries( vm util reader analyzer values eval)

add_executable( lllm lllm.cpp )

target_link_libraries(lllm vm)

# testing
add_executable( test_1_reader   test_1_reader.cpp   )
add_executable( test_2_analyzer test_2_analyzer.cpp )
add_executable( test_3_eval     test_3_eval.cpp     )
#add_executable( test_4_jit      test_4_jit.cpp      )

target_link_libraries( test_1_reader   reader    )
target_link_libraries( test_2_analyzer analyzer  )
target_link_libraries( test_3_eval     eval reader analyzer vm )
#target_link_libraries( test_4_jit      jit       )


