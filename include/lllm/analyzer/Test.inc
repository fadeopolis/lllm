
#ifndef VISITOR
#	error "NO VISITOR MACRO WAS DEFINED
#endif

VISITOR( Int    )
VISITOR( Float  )
VISITOR( String )

#undef VISITOR

