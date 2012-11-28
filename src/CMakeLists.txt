
add_subdirectory( util  )
add_subdirectory( sexpr )
add_subdirectory( ast   )
add_subdirectory( value )

add_library( lllm lllm.cpp Reader.cpp Analyzer.cpp Evaluator.cpp EscapeAnalyzer.cpp Jit.cpp Builtins.cpp GlobalScope.cpp )
add_library( jit  STATIC IMPORTED )
set_property( TARGET jit PROPERTY IMPORTED_LOCATION /home/fader/Desktop/libjit.a ) 

target_link_libraries( lllm sexpr ast value gc jit
 ## LLVM requires a LOT of libs
#	LLVMX86Disassembler LLVMX86AsmParser LLVMX86AsmPrinter LLVMX86CodeGen LLVMX86Desc LLVMX86Utils LLVMX86Info
#	LLVMSelectionDAG LLVMAsmPrinter LLVMMCParser LLVMJIT LLVMExecutionEngine LLVMCodeGen LLVMScalarOpts LLVMInstCombine
#	LLVMTransformUtils LLVMipa LLVMAnalysis LLVMTarget LLVMMC LLVMCore LLVMSupport LLVMInterpreter

#	LLVMXCoreCodeGen LLVMTableGen LLVMRuntimeDyld LLVMObject LLVMMCDisassembler LLVMXCoreDesc LLVMXCoreInfo
#	LLVMPTXDesc LLVMPTXInfo LLVMPTXAsmPrinter LLVMLinker LLVMipo LLVMInterpreter LLVMInstrumentation LLVMJIT#
#	LLVMExecutionEngine LLVMDebugInfo
#	LLVMBitWriter LLVMX86Disassembler LLVMX86AsmParser LLVMX86CodeGen LLVMX86Desc LLVMX86AsmPrinter LLVMX86Utils
#	LLVMX86Info LLVMAsmParser LLVMArchive LLVMBitReader LLVMSelectionDAG LLVMAsmPrinter LLVMMCParser LLVMCodeGen
#	LLVMScalarOpts LLVMInstCombine LLVMTransformUtils LLVMipa LLVMAnalysis LLVMTarget LLVMCore LLVMMC LLVMSupport

	pthread ffi dl m
 )

add_executable( repl repl.cpp )
target_link_libraries( repl lllm gc )

## testing
add_executable( test_1_reader   test_1_reader.cpp   )
add_executable( test_2_analyzer test_2_analyzer.cpp )
add_executable( test_3_eval     test_3_eval.cpp     )
add_executable( test_4_jit      test_4_jit.cpp )

target_link_libraries( test_1_reader   lllm )
target_link_libraries( test_2_analyzer lllm )
target_link_libraries( test_3_eval     lllm )
target_link_libraries( test_4_jit      lllm )


