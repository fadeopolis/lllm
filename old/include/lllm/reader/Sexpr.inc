
#ifndef LLLM_VISITOR
#	error "No VISITOR macro was defined"
#endif

LLLM_VISITOR( Int    )
LLLM_VISITOR( Real   )
LLLM_VISITOR( Char   )
LLLM_VISITOR( String )
LLLM_VISITOR( Symbol )
LLLM_VISITOR( List   )

#undef LLLM_VISITOR

