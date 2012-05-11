
#include "lllm/Builtins.hpp"
#include "lllm/Evaluator.hpp"
#include "lllm/ast/AstIO.hpp"
#include "lllm/value/ValueIO.hpp"
#include "lllm/util/fail.hpp"
#include "lllm/util/util_io.hpp"

#include <iostream>

using namespace lllm;
using namespace lllm::value;
using namespace lllm::util;

static SourceLocation builtin_location("*builtin*");
	
Builtins* Builtins::INSTANCE;

Builtins& Builtins::get() {
	if ( !INSTANCE ) INSTANCE = new Builtins();

	return *INSTANCE;
}

bool Builtins::lookup( const util::InternedString& name, ast::VariablePtr* dst ) {
	value::ValuePtr val;

	if ( lookup( name, &val ) ) {
		*dst = new ast::Variable( builtin_location, name, value::typeOf( val ), true );
		return true;
	} else {
		return false;
	}
}
bool Builtins::lookup( const util::InternedString& name, value::ValuePtr* dst ) {
	auto lb = data.lower_bound( name );

	if ( lb != data.end() && lb->first == name ) {
		*dst = lb->second;
		return true;
	} else {
		return false;
	}
}
bool Builtins::contains( const util::InternedString& name ) {
	auto lb = data.lower_bound( name );

	if ( lb != data.end() && lb->first == name ) {
		return true;
	} else {
		return false;
	}
}


static inline LambdaPtr mk_builtin_fn( CStr name, void* fn, size_t arity, TypeSet types ) {
	std::vector<ast::VariablePtr> params;

	for ( size_t i = 0; i < arity; i++ ) {
		params.push_back( new ast::Variable( builtin_location, "arg", types, true ) );
	}

	ast::Lambda* ast = new ast::Lambda( builtin_location, nullptr, name, params, std::vector<ast::VariablePtr>(), nullptr );

	return Lambda::alloc( ast, (Lambda::FnPtr) fn );
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
		case Type::Int:  
			switch ( typeOf( b ) ) {
				case Type::Int:  return number( static_cast<IntPtr>( a )->value + static_cast<IntPtr> ( b )->value );
				case Type::Real: return number( static_cast<IntPtr>( a )->value + static_cast<RealPtr>( b )->value );
				default: LLLM_FAIL( "builtin function '+' expects a number as second argument, not a " << b );
			}
		case Type::Real:
			switch ( typeOf( b ) ) {
				case Type::Int:  return number( static_cast<RealPtr>( a )->value + static_cast<IntPtr> ( b )->value );
				case Type::Real: return number( static_cast<RealPtr>( a )->value + static_cast<RealPtr>( b )->value );
				default: LLLM_FAIL( "builtin function '+' expects a number as second argument, not a " << b );
			}
		default: LLLM_FAIL( "builtin function '+' expects a number as first argument, not a " << a );
	}
}
static ValuePtr builtin_minus( LambdaPtr fn, ValuePtr a, ValuePtr b ) {
	switch ( typeOf( a ) ) {
		case Type::Int:  
			switch ( typeOf( b ) ) {
				case Type::Int:  return number( static_cast<IntPtr>( a )->value - static_cast<IntPtr> ( b )->value );
				case Type::Real: return number( static_cast<IntPtr>( a )->value - static_cast<RealPtr>( b )->value );
				default: LLLM_FAIL( "builtin function '-' expects a number as second argument, not a " << b );
			}
		case Type::Real:
			switch ( typeOf( b ) ) {
				case Type::Int:  return number( static_cast<RealPtr>( a )->value - static_cast<IntPtr> ( b )->value );
				case Type::Real: return number( static_cast<RealPtr>( a )->value - static_cast<RealPtr>( b )->value );
				default: LLLM_FAIL( "builtin function '-' expects a number as second argument, not a " << b );
			}
		default: LLLM_FAIL( "builtin function '-' expects a number as first argument, not a " << a );
	}
}
static ValuePtr builtin_equal( LambdaPtr fn, ValuePtr a, ValuePtr b ) {
	return equal( a, b ) ? ((ValuePtr)symbol("true")) : ((ValuePtr)nil);
}
static ValuePtr builtin_print( LambdaPtr fn, ValuePtr v ) {
	std::cout << v;
	return nullptr;
}
static ValuePtr builtin_println( LambdaPtr fn, ValuePtr v ) {
	std::cout << v << std::endl;
	return nullptr;
}

Builtins::Builtins() {
	#define BUILTIN( NAME, VAL )                 data[NAME] = VAL;
	#define BUILTIN_FN( NAME, FN, ARITY, TYPES ) data[NAME] = mk_builtin_fn( NAME, (void*) FN, ARITY, TYPES );

	// ***** CONSTANTS
	BUILTIN( "nil",     nil );
	BUILTIN( "true",    number(1) );
	BUILTIN( "false",   nil );
	//**** LISTS
	BUILTIN_FN( "cons",    builtin_cons,    2, TypeSet::Cons() );
	BUILTIN_FN( "car",     builtin_car,     1, TypeSet::all() );
	BUILTIN_FN( "cdr",     builtin_cdr,     1, TypeSet::Cons() );
	//**** ARITHMETIC
	BUILTIN_FN( "+",       builtin_plus,    2, TypeSet::Number() );
	BUILTIN_FN( "-",       builtin_minus,   2, TypeSet::Number() );
	//**** LOGIC
	//**** EQUALITY
	BUILTIN_FN( "=",       builtin_equal,   2, TypeSet::all() );
	//**** IO
	BUILTIN_FN( "print",   builtin_print,   1, TypeSet::Nil() );
	BUILTIN_FN( "println", builtin_println, 1, TypeSet::Nil() );
}

// (define sum (lambda (a b) (if (= a 0) b (sum (- a 1) (+ 1 b)))))


