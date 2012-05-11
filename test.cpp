
#include <jit/jit.h>
#include <jit/jit-dump.h>

#include <stdio.h>

typedef struct Arg {
	long _1;
	long _2;
} Arg;

typedef long (*fn_t)( Arg* );

int main() {
	printf( "BUILT AT " __TIME__ " ON " __DATE__ "\n" );
	printf( "STARTING LIBJIT TEST\n" );

	// init jit
	jit_context_t context = jit_context_create();

	// lock jit
	jit_context_build_start(context);

	// construct type
	jit_type_t fields[] = { jit_type_long, jit_type_long };
	jit_type_t struct_t = jit_type_create_struct( fields, 2, 1 );
	jit_type_t ptr_t    = jit_type_create_pointer( jit_type_long, 1 );

	jit_type_t params[]  = { ptr_t };
	jit_type_t signature = jit_type_create_signature( jit_abi_cdecl, jit_type_long, params, 1, 1 );

	// construct function
	jit_function_t function = jit_function_create(context, signature);

	// construct function body
	jit_value_t x   = jit_value_get_param( function, 0 );
//	jit_value_t x_1 = jit_insn_load_elem( function, x, 0, jit_type_long );
	jit_value_t x_1 = jit_insn_load_relative( function, x, 0, jit_type_long );
	jit_value_t out = jit_value_create_long_constant( function, jit_type_long, 42 );

	jit_insn_return( function, x_1 );
//	jit_insn_return( function, out );

//	printf("BEFORE COMPILE -----------------------------------------------------\n");
//	jit_dump_function( stdout, function, "FUNC" );
//	printf("\n");

	// compile function
	if ( jit_function_compile(function) == 0 ) {
		printf( "Could not compile\n" );
		return 1;
	}

//	printf("AFTER  COMPILE -----------------------------------------------------\n");
//	jit_dump_function( stdout, function, "FUNC" );
//	printf("\n");

	// unlock jit
	jit_context_build_end(context);

	// /execute function and print result
	Arg arg = { 42, 43 };

	fn_t fn = (fn_t) jit_function_to_closure( function );

	printf( "CALL -> %ld\n", fn( &arg ) );

	// destroy jit
	jit_context_destroy( context );

	printf( "FINISHED LIBJIT TEST\n" );
	return 0;
}
