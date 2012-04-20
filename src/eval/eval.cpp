
#include "eval.hpp"
#include "Env.hpp"
#include "values.hpp"
#include "fail.hpp"

#include <cassert>
#include <cstring>
#include <string>
#include <sstream>

#include <iostream>

using namespace std;
using namespace lllm;
using namespace lllm::val;

static EnvPtr BUILTIN_ENV = Env::make()
	->store( "nil",   val::nil() )
	->store( "true" , val::True  )
	->store( "false", val::False )
	;

EnvPtr lllm::builtins() { return BUILTIN_ENV; }

static ValuePtr evalSym( SymbolPtr val, EnvPtr env );
static ValuePtr evalCons( ConsPtr   val, EnvPtr env );

static ValuePtr apply( ValuePtr  v, ValuePtr args );
static ValuePtr apply( LambdaPtr l, ValuePtr args );
static ValuePtr apply( ThunkPtr  t, ValuePtr args );

static ValuePtr force( ThunkPtr thunk );

ValuePtr lllm::eval( ValuePtr val, EnvPtr env ) {
	switch ( typeOf( val ) ) {
		case Type::Int:
			return val;
		case Type::Real:
			return val;
		case Type::Char:
			return val;
		case Type::String:
			return val;
		case Type::Symbol:
			return evalSym( static_cast<SymbolPtr>( val ), env );
		case Type::Cons:
			return evalCons( static_cast<ConsPtr>( val ), env );
		case Type::Nil:
			return val;
		case Type::Lambda:
			return val;
		case Type::Thunk:
			return force( static_cast<ThunkPtr>( val ) ); // force thunk
		case Type::Ref:
			return val;
	}
	
	return nullptr;
}

ValuePtr evalSym( SymbolPtr val, EnvPtr env ) {
	ValuePtr* ptr;
	if ( !env->lookup( val->value, ptr ) ) {
		LLLM_FAIL( "Unknown symbol: '" << val->value << "'" );
	} else {
		return *ptr;
	}
}
ValuePtr evalCons( ConsPtr val, EnvPtr env ) {
	ValuePtr car = val->car;
	ValuePtr cdr = val->cdr;

	Type carType = typeOf( car );

	if ( carType == Type::Symbol ) {
		SymbolPtr sym = static_cast<SymbolPtr>( car );

		// is it a special form?
		if ( strcmp( "quote", sym->value ) == 0 ) {
			return val::car( cdr );
		} else if ( strcmp( "define", sym->value ) == 0 ) {
			LLLM_FAIL( "SPECIAL FORM define NOT IMPLEMENTED YET" );
		} else if ( strcmp( "if", sym->value ) == 0 ) {
			ValuePtr test     = val::car( cdr );
			ValuePtr thenPart = val::cadr( cdr );
			ValuePtr elsePart = val::caddr( cdr );

			if ( eval( test, env ) ) {
				return eval( thenPart, env );
			} else {
				return eval( elsePart, env );
			}
		} else if ( strcmp( "lambda", sym->value ) == 0 ) {
			LLLM_FAIL( "SPECIAL FORM lambda NOT IMPLEMENTED YET" );
		}
	}

	LLLM_FAIL( "NOT IMPLEMENTED YET" );
}

/*
ValuePtr apply( ConsPtr application ) {
	ValuePtr car = application->car;
	ValuePtr cdr  = application->cdr;

	switch ( typeOf( car ) ) {
		case Type::Lambda:
			return apply( static_cast<LambdaPtr>( car ), cdr );
		case Type::Thunk:
			return apply( static_cast<ThunkPtr>( car ), cdr );
		default:
			LLLM_FAIL( "Cannot apply '" << car << "', it is not a function" );
	}
}
*/
ValuePtr force( ThunkPtr thunk ) {
	LLLM_FAIL( "NOT IMPLEMENTED YET" );
}


