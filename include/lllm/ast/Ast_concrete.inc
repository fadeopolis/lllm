#ifndef LLLM_VISITOR
#	error "No VISITOR macro was defined"
#endif

LLLM_VISITOR( Nil         )
LLLM_VISITOR( Int         )
LLLM_VISITOR( Real        )
LLLM_VISITOR( Char        )
LLLM_VISITOR( String      )
LLLM_VISITOR( Variable    )
//LLLM_VISITOR( Builtin     )
//LLLM_VISITOR( Global      )
//LLLM_VISITOR( Parameter   )
//LLLM_VISITOR( Captured    )
//LLLM_VISITOR( Local       )
LLLM_VISITOR( Quote       )
LLLM_VISITOR( If          )
LLLM_VISITOR( Do          )
LLLM_VISITOR( Let         )
LLLM_VISITOR( LetStar     )
LLLM_VISITOR( Lambda      )
LLLM_VISITOR( Define      )
LLLM_VISITOR( Application )

#undef LLLM_VISITOR
