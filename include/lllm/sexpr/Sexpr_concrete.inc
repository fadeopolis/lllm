
#ifndef LLLM_VISITOR
#	error "No VISITOR macro was defined"
#endif

LLLM_VISITOR( Int ,   long                 )
LLLM_VISITOR( Real,   double               )
LLLM_VISITOR( Char,   char                 )
LLLM_VISITOR( String, util::CStr           )
LLLM_VISITOR( Symbol, util::InternedString )
LLLM_VISITOR( List,   SexprVector          )

#undef LLLM_VISITOR

