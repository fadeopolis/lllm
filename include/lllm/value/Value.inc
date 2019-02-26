
#ifndef LLLM_VISIT_ROOT
#  define LLLM_VISIT_ROOT(TYPE) LLLM_VISIT_ABSTRACT(TYPE)
#endif

#ifndef LLLM_VISIT_ABSTRACT
#  ifndef LLLM_VISIT
#    error "No VISITOR macro was defined"
#  endif
#  define LLLM_VISIT_ABSTRACT(TYPE) LLLM_VISIT(TYPE)
#endif

#ifndef LLLM_VISIT_CONCRETE
#  ifndef LLLM_VISIT
#    error "No VISITOR macro was defined"
#  endif
#  define LLLM_VISIT_CONCRETE(TYPE) LLLM_VISIT(TYPE)
#endif

LLLM_VISIT_ROOT( Value )
	LLLM_VISIT_ABSTRACT( List )
		LLLM_VISIT_CONCRETE( Nil )
		LLLM_VISIT_CONCRETE( Cons )
	LLLM_VISIT_ABSTRACT( Number )
		LLLM_VISIT_CONCRETE( Int )
		LLLM_VISIT_CONCRETE( Real )
	LLLM_VISIT_CONCRETE( Char )
	LLLM_VISIT_CONCRETE( String )
	LLLM_VISIT_CONCRETE( Symbol )
	LLLM_VISIT_CONCRETE( Ref )
	// We have a special encoding of Lamba values.
	// So here they are not considered concrete values
	LLLM_VISIT_ABSTRACT( Lambda )

#ifdef LLLM_VISIT_CONCRETE
#  undef LLLM_VISIT
#endif

#undef LLLM_VISIT_ROOT
#undef LLLM_VISIT_ABSTRACT
#undef LLLM_VISIT_CONCRETE
