
#ifndef LLLM_VISITOR
#	error "No VISITOR macro was defined"
#endif

// atoms
LLLM_VISITOR( Nil         )
LLLM_VISITOR( Int         )
LLLM_VISITOR( Real        )
LLLM_VISITOR( Char        )
LLLM_VISITOR( String      )

// variables
LLLM_VISITOR( Variable    )
LLLM_VISITOR( Builtin     )
//LLLM_VISITOR( GlobalVariable   )
//LLLM_VISITOR( Parameter        )
//LLLM_VISITOR( LocalVariable    )
//LLLM_VISITOR( CapturedVariable )

// special forms
LLLM_VISITOR( Quote       )
LLLM_VISITOR( If          )
LLLM_VISITOR( Do          )
LLLM_VISITOR( Define      )
LLLM_VISITOR( Let         )

// function application
LLLM_VISITOR( Application )

// lambdas come last to allow for variable arities
LLLM_VISITOR( Lambda      )

#undef LLLM_VISITOR
