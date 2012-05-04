
#include "lllm/builtins.hpp"
#include "lllm/eval.hpp"
#include "lllm/eval/Env.hpp"
#include "lllm/analyzer/Ast.hpp"
#include "lllm/values/ValueIO.hpp"
#include "lllm/util/fail.hpp"

#include <iostream>

using namespace lllm;
using namespace lllm::eval;
using namespace lllm::value;

static reader::SourceLocation builtin_location("*builtin*");

BuiltinScopePtr BuiltinScope::INSTANCE;

BuiltinScopePtr BuiltinScope::builtins() {
	if ( !INSTANCE ) {
		INSTANCE = new BuiltinScope();
	}
	return INSTANCE;
}

analyzer::VariablePtr BuiltinScope::get( const util::InternedString& name ) {
	ValuePtr val;
	if ( _env->lookup( name, &val ) ) {
		return new analyzer::Builtin( name, typeOf( val ) );
	} else {
		return nullptr;
	}
}
		
EnvPtr BuiltinScope::env() const {
	return _env;
}

static inline LambdaPtr mk_builtin_fn( void* fn, size_t arity ) {
	std::vector<analyzer::ParameterPtr> params;

	for ( size_t i = 0; i < arity; i++ ) {
		params.push_back( 
			new analyzer::Parameter( builtin_location, nullptr )
		);
	}

	analyzer::Lambda* ast = new analyzer::Lambda( builtin_location, nullptr, params, std::vector<analyzer::CapturedPtr>(), nullptr );

	Lambda* clojure = Lambda::alloc( ast->arity(), 0 );
	
	clojure->fun = (Lambda::FnPtr) fn;
	clojure->ast = ast;

	return clojure;
}

static value::ValuePtr builtin_cons( LambdaPtr fn, ValuePtr car, ValuePtr cdr ) {
	if ( Value::isList( cdr ) ) {
		return cons( car, static_cast<ListPtr>( cdr ) );
	} else {
		LLLM_FAIL( "builtin function 'cons' expects a list as second argument, not a " << cdr );
	}
}
static value::ValuePtr builtin_car( LambdaPtr fn, ValuePtr v ) {
	if ( ConsPtr cons = Value::asCons( v ) ) {
		return cons->car;
	} else {
		LLLM_FAIL( "builtin function 'car' expects a cons as first argument, not a " << v );
	}
}
static ValuePtr builtin_cdr( LambdaPtr fn, ValuePtr v ) {
	if ( ConsPtr cons = Value::asCons( v ) ) {
		return cons->cdr;
	} else {
		LLLM_FAIL( "builtin function 'cdr' expects a cons as first argument, not a " << v );
	}
}
static ValuePtr builtin_plus( LambdaPtr fn, ValuePtr a, ValuePtr b ) {
	switch ( typeOf( a ) ) {
		case Value::Int:  
			switch ( typeOf( b ) ) {
				case Value::Int:  return number( static_cast<IntPtr>( a )->value + static_cast<IntPtr> ( b )->value );
				case Value::Real: return number( static_cast<IntPtr>( a )->value + static_cast<RealPtr>( b )->value );
				default: LLLM_FAIL( "builtin function '+' expects a number as second argument, not a " << b );
			}
		case Value::Real:
			switch ( typeOf( b ) ) {
				case Value::Int:  return number( static_cast<RealPtr>( a )->value + static_cast<IntPtr> ( b )->value );
				case Value::Real: return number( static_cast<RealPtr>( a )->value + static_cast<RealPtr>( b )->value );
				default: LLLM_FAIL( "builtin function '+' expects a number as second argument, not a " << b );
			}
		default: LLLM_FAIL( "builtin function '+' expects a number as first argument, not a " << a );
	}
}
static ValuePtr builtin_minus( LambdaPtr fn, ValuePtr a, ValuePtr b ) {
	switch ( typeOf( a ) ) {
		case Value::Int:  
			switch ( typeOf( b ) ) {
				case Value::Int:  return number( static_cast<IntPtr>( a )->value - static_cast<IntPtr> ( b )->value );
				case Value::Real: return number( static_cast<IntPtr>( a )->value - static_cast<RealPtr>( b )->value );
				default: LLLM_FAIL( "builtin function '-' expects a number as second argument, not a " << b );
			}
		case Value::Real:
			switch ( typeOf( b ) ) {
				case Value::Int:  return number( static_cast<RealPtr>( a )->value - static_cast<IntPtr> ( b )->value );
				case Value::Real: return number( static_cast<RealPtr>( a )->value - static_cast<RealPtr>( b )->value );
				default: LLLM_FAIL( "builtin function '-' expects a number as second argument, not a " << b );
			}
		default: LLLM_FAIL( "builtin function '-' expects a number as first argument, not a " << a );
	}
}
static ValuePtr builtin_equal( LambdaPtr fn, ValuePtr a, ValuePtr b ) {
	return equal( a, b ) ? ((ValuePtr)symbol("true")) : ((ValuePtr)nil());
}

BuiltinScope::BuiltinScope() {
	_env = Env::make( "nil",   nil() )
	           ->put( "true",  symbol("true") )
	           ->put( "false", nil() )
	           ->put( "cons",  mk_builtin_fn( (void*) builtin_cons,  2 ) )
	           ->put( "car",   mk_builtin_fn( (void*) builtin_car,   1 ) )
	           ->put( "cdr",   mk_builtin_fn( (void*) builtin_cdr,   1 ) )
	           ->put( "+",     mk_builtin_fn( (void*) builtin_plus,  2 ) )
	           ->put( "-",     mk_builtin_fn( (void*) builtin_minus, 2 ) )
	           ->put( "=",     mk_builtin_fn( (void*) builtin_equal, 2 ) )
//	           ->put( "", )
	;
}

// (define sum (lambda (a b) (if (= a 0) b (sum (- a 1) (+ 1 b)))))








