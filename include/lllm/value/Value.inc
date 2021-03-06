
#ifndef LLLM_VISITOR
#	error "No VISITOR macro was defined"
#endif

LLLM_VISITOR( Value  )
	LLLM_VISITOR( List   )
		LLLM_VISITOR( Nil    )
		LLLM_VISITOR( Cons   )
	LLLM_VISITOR( Number )
		LLLM_VISITOR( Int    )
		LLLM_VISITOR( Real   )
	LLLM_VISITOR( Char   )
	LLLM_VISITOR( String )
	LLLM_VISITOR( Symbol )
	LLLM_VISITOR( Ref    )
	LLLM_VISITOR( Lambda )

#undef LLLM_VISITOR

