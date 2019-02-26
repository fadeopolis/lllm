#ifndef NAME
#	error "No name was provided, define macro NAME"
#endif
#ifndef INT_INT
#	error "No implementation for case (Int,Int) was provided, define macro INT_INT( A, B )"
#endif
#ifndef INT_REAL
#	error "No implementation for case (Int,Real) was provided, define macro INT_INT( A, B )"
#endif
#ifndef REAL_INT
#	error "No implementation for case (Real,Int) was provided, define macro INT_INT( A, B )"
#endif
#ifndef REAL_REAL
#	error "No implementation for case (Real,Real) was provided, define macro INT_INT( A, B )"
#endif

#undef NAME
#undef INT_INT
#undef INT_REAL
#undef REAL_INT
#undef REAL_REAL
