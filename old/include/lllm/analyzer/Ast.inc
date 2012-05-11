#ifndef LLLM_VISITOR
#	error "No VISITOR macro was defined"
#endif

LLLM_VISITOR( Ast         )

// atoms
//LLLM_VISITOR( Atom        )
LLLM_VISITOR( Nil         )
LLLM_VISITOR( Int         )
LLLM_VISITOR( Real        )
LLLM_VISITOR( Char        )
LLLM_VISITOR( String      )

// variables
LLLM_VISITOR( Variable    )
LLLM_VISITOR( Builtin     )
LLLM_VISITOR( Global      )
LLLM_VISITOR( Parameter   )
LLLM_VISITOR( Captured    )
LLLM_VISITOR( Local       )

// special forms
//LLLM_VISITOR( SpecialForm )
LLLM_VISITOR( Quote       )
LLLM_VISITOR( If          )
LLLM_VISITOR( Do          )
LLLM_VISITOR( Let         )
LLLM_VISITOR( Lambda      )
LLLM_VISITOR( Define      )

// function application
LLLM_VISITOR( Application )

#undef LLLM_VISITOR
