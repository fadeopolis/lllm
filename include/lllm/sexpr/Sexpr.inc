
#ifndef LLLM_VISIT_ROOT
#  define LLLM_VISIT_ROOT(TYPE, VALUE) LLLM_VISIT_ABSTRACT(TYPE, VALUE)
#endif

#ifndef LLLM_VISIT_ABSTRACT
#  ifndef LLLM_VISIT
#    error "No VISITOR macro was defined"
#  endif
#  define LLLM_VISIT_ABSTRACT(TYPE, VALUE) LLLM_VISIT(TYPE, VALUE)
#endif

#ifndef LLLM_VISIT_CONCRETE
#  ifndef LLLM_VISIT
#    error "No VISITOR macro was defined"
#  endif
#  define LLLM_VISIT_CONCRETE(TYPE, VALUE) LLLM_VISIT(TYPE, VALUE)
#endif

LLLM_VISIT_ROOT( Sexpr, void )
	LLLM_VISIT_ABSTRACT( Atom, void )
    LLLM_VISIT_CONCRETE( Int , long )
    LLLM_VISIT_CONCRETE( Real, double )
    LLLM_VISIT_CONCRETE( Char, char )
    LLLM_VISIT_CONCRETE( String, util::CStr )
    LLLM_VISIT_CONCRETE( Symbol, util::InternedString )
  LLLM_VISIT_CONCRETE( List, SexprVector )

#ifdef LLLM_VISIT_CONCRETE
#  undef LLLM_VISIT
#endif

#undef LLLM_VISIT_ROOT
#undef LLLM_VISIT_ABSTRACT
#undef LLLM_VISIT_CONCRETE
