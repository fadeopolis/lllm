
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

static ValuePtr TRUE = nullptr;

Builtins& Builtins::get() {
	if ( !INSTANCE ) INSTANCE = new Builtins();

	return *INSTANCE;
}

bool Builtins::lookup( const util::InternedString& name, ast::AstPtr*      dst ) {
	auto lb = data.lower_bound( name );

	if ( lb != data.end() && lb->first == name ) {
		*dst = lb->second.first->ast;
		return true;
	} else {
		return false;
	}
}
bool Builtins::lookup( const util::InternedString& name, ast::VariablePtr* dst ) {
	auto lb = data.lower_bound( name );

	if ( lb != data.end() && lb->first == name ) {
		*dst = lb->second.first;
		return true;
	} else {
		return false;
	}
}
bool Builtins::lookup( const util::InternedString& name, value::ValuePtr* dst ) {
	auto lb = data.lower_bound( name );

	if ( lb != data.end() && lb->first == name ) {
		*dst = lb->second.second;
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

//***** LISTS **********************************************************************************************************
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

//***** ARITHMETIC *****************************************************************************************************

#define BUILTIN_BINARY_ARITH( OP, A, B ) 																				\
	switch ( typeOf( A ) ) {																							\
		case Type::Int:  																								\
			switch ( typeOf( B ) ) {																					\
				case Type::Int:  return number( static_cast<IntPtr>( A )->value OP static_cast<IntPtr> ( B )->value );	\
				case Type::Real: return number( static_cast<IntPtr>( A )->value OP static_cast<RealPtr>( B )->value );	\
				default: LLLM_FAIL( "builtin function '" #OP "' expects a number as second argument, not a " << B );	\
			}																											\
		case Type::Real:																								\
			switch ( typeOf( B ) ) {																					\
				case Type::Int:  return number( static_cast<RealPtr>( A )->value OP static_cast<IntPtr> ( B )->value );	\
				case Type::Real: return number( static_cast<RealPtr>( A )->value OP static_cast<RealPtr>( B )->value );	\
				default: LLLM_FAIL( "builtin function '" #OP "' expects a number as second argument, not a " << B );	\
			}																											\
		default: LLLM_FAIL( "builtin function '" #OP "' expects a number as first argument, not a " << A );				\
	}		

static ValuePtr builtin_add( LambdaPtr fn, ValuePtr a, ValuePtr b ) { BUILTIN_BINARY_ARITH( +, a, b ) }
static ValuePtr builtin_sub( LambdaPtr fn, ValuePtr a, ValuePtr b ) { BUILTIN_BINARY_ARITH( -, a, b ) }
static ValuePtr builtin_mul( LambdaPtr fn, ValuePtr a, ValuePtr b ) { BUILTIN_BINARY_ARITH( *, a, b ) }
static ValuePtr builtin_div( LambdaPtr fn, ValuePtr a, ValuePtr b ) { BUILTIN_BINARY_ARITH( /, a, b ) }

//***** LOGIC **********************************************************************************************************

static ValuePtr builtin_and( LambdaPtr fn, ValuePtr a, ValuePtr b ) { return (a && b) ? TRUE : nullptr; }
static ValuePtr builtin_or ( LambdaPtr fn, ValuePtr a, ValuePtr b ) { return (a || b) ? TRUE : nullptr; }

//***** RELATIONS ******************************************************************************************************
static ValuePtr builtin_equal( LambdaPtr fn, ValuePtr a, ValuePtr b ) {
	return equal( a, b ) ? TRUE : nullptr;
}
#define BUILTIN_BINARY_CMP( OP, A, B )                                                                                           \
	switch ( typeOf( A ) ) {                                                                                                     \
		case Type::Int:                                                                                                          \
			switch ( typeOf( B ) ) {                                                                                             \
				case Type::Int:  return ((static_cast<IntPtr>( A )->value) OP (static_cast<IntPtr> ( B )->value)) ? TRUE : nullptr;  \
				case Type::Real: return ((static_cast<IntPtr>( A )->value) OP (static_cast<RealPtr>( B )->value)) ? TRUE : nullptr;  \
				default: LLLM_FAIL( "builtin function '" #OP "' expects a number as second argument, not a " << B );             \
			}                                                                                                                    \
		case Type::Real:                                                                                                         \
			switch ( typeOf( B ) ) {                                                                                             \
				case Type::Int:  return ((static_cast<RealPtr>( A )->value) OP (static_cast<IntPtr> ( B )->value)) ? TRUE : nullptr; \
				case Type::Real: return ((static_cast<RealPtr>( A )->value) OP (static_cast<RealPtr>( B )->value)) ? TRUE : nullptr; \
				default: LLLM_FAIL( "builtin function '" #OP "' expects a number as second argument, not a " << B );             \
			}                                                                                                                    \
		default: LLLM_FAIL( "builtin function '" #OP "' expects a number as first argument, not a " << A );                      \
	}	

static ValuePtr builtin_lt( LambdaPtr fn, ValuePtr a, ValuePtr b ) { BUILTIN_BINARY_CMP( <,  a, b ) }
static ValuePtr builtin_gt( LambdaPtr fn, ValuePtr a, ValuePtr b ) { BUILTIN_BINARY_CMP( >,  a, b ) }
static ValuePtr builtin_le( LambdaPtr fn, ValuePtr a, ValuePtr b ) { BUILTIN_BINARY_CMP( <=, a, b ) }
static ValuePtr builtin_ge( LambdaPtr fn, ValuePtr a, ValuePtr b ) { BUILTIN_BINARY_CMP( >=, a, b ) }

//***** REFS ***********************************************************************************************************

static ValuePtr builtin_ref( LambdaPtr fn ) {
	return value::ref();
}
static ValuePtr builtin_get( LambdaPtr fn, ValuePtr ref ) {
	if ( RefPtr r = Value::asRef( ref ) ) {
		return r->get();
	} else {
		LLLM_FAIL( "builtin function 'get' expects a ref as first argument, not a " << ref );
	}
}
static ValuePtr builtin_set( LambdaPtr fn, ValuePtr ref, ValuePtr v ) {
	if ( RefPtr r = Value::asRef( ref ) ) {
		return r->set( v );
	} else {
		LLLM_FAIL( "builtin function 'set' expects a ref as first argument, not a " << ref );
	}
}

//***** IO *************************************************************************************************************
static ValuePtr builtin_print( LambdaPtr fn, ValuePtr v ) {
	struct Visitor {
		void visit( ValuePtr  v, std::ostream& os ) const { os << v; }
		void visit( CharPtr   v, std::ostream& os ) const { os << v->value; }
		void visit( StringPtr v, std::ostream& os ) const { os << v->value; }
		void visit( SymbolPtr v, std::ostream& os ) const { os << "'" << v->value; }
	};

	visit<void,Visitor,std::ostream&>( v, Visitor(), std::cout );
	return nullptr;
}
static ValuePtr builtin_println( LambdaPtr fn, ValuePtr v ) {
	builtin_print( fn, v );
	std::cout << std::endl;
	return nullptr;
}

const ValuePtr Builtins::CLEAR_MARK = value::symbol("__BUILTIN_CLEAR_MARK__");

//***** SETUP **********************************************************************************************************

inline size_t numArgs() { return 0; }
template<typename T, typename... Ts> 
inline size_t numArgs( const T& t, const Ts&... ts ) { return 1 + numArgs( ts... ); }

inline ast::LambdaPtr doMakeBuiltinFn( CStr name, TypeSet returnT, size_t arity ) {
	std::vector<ast::VariablePtr> params;

	for ( size_t i = 0; i < arity; i++ ) {
		params.push_back( ast::Variable::makeParameter( builtin_location, "arg" ) );
	}

	return new ast::Lambda( builtin_location, name, params, std::vector<ast::VariablePtr>(), nullptr );
}

inline void initEscape( ast::LambdaPtr lambda, ast::Lambda::Iterator it ) {}
template<typename... ParamEscapes>
inline void initEscape( ast::LambdaPtr lambda, ast::Lambda::Iterator it, EscapeStatus p1, ParamEscapes... ps ) {
	lambda->paramEscape( it, p1 );
	initEscape( lambda, ++it, ps... );
}

template<typename... ParamEscapes>
inline ast::LambdaPtr makeBuiltinFn( CStr name, TypeSet returnT, ParamEscapes... ps ) {
	auto ast = doMakeBuiltinFn( name, returnT, numArgs( ps... ) );

	initEscape( ast, ast->params_begin(), ps... );

	return ast;
}

Builtins::Builtins() {
	#define BUILTIN( NAME, VAL ) (void) ({                                         \
		auto var = ast::Variable::makeGlobal( builtin_location, (NAME), nullptr ); \
		auto val = (VAL);                                                          \
		data.insert( std::make_pair( (NAME), std::make_pair( var, val ) ) );       \
		nullptr;                                                                   \
	})

	#define BUILTIN_FN( NAME, FN, RETURN, ESCAPES... ) (void) ({               \
		auto ast = makeBuiltinFn( NAME, RETURN, ## ESCAPES );                  \
		auto var = ast::Variable::makeGlobal( builtin_location, (NAME), ast ); \
		auto fun = Lambda::alloc( ast, (Lambda::FnPtr) FN );                   \
		data.insert( std::make_pair( (NAME), std::make_pair( var, fun ) ) );   \
		nullptr;                                                               \
	})

	EscapeStatus NO_ESCAPE        = EscapeStatus::NO_ESCAPE;
//	EscapeStatus ESCAPE_AS_PARAM  = EscapeStatus::ESCAPE_AS_PARAM;
	EscapeStatus ESCAPE_AS_RETURN = EscapeStatus::ESCAPE_AS_RETURN;
	EscapeStatus ESCAPE_GLOBAL    = EscapeStatus::ESCAPE_GLOBAL;

	BUILTIN_FN( "cons",    builtin_cons,    TypeSet::Cons(), ESCAPE_AS_RETURN, ESCAPE_AS_RETURN );

//	TRUE = symbol("true");
	TRUE = number(1);

	// ***** CONSTANTS
	BUILTIN( "nil",     nil   );
	BUILTIN( "true",    TRUE  );
	BUILTIN( "false",   False );
	BUILTIN( "clear",   CLEAR_MARK );
	// ***** LISTS
	BUILTIN_FN( "cons",    builtin_cons,    TypeSet::Cons(), ESCAPE_AS_RETURN, ESCAPE_AS_RETURN );
	BUILTIN_FN( "car",     builtin_car,     TypeSet::all(),  NO_ESCAPE );
	BUILTIN_FN( "cdr",     builtin_cdr,     TypeSet::Cons(), NO_ESCAPE );
	// ***** ARITHMETIC
	BUILTIN_FN( "+",       builtin_add,     TypeSet::Number(), NO_ESCAPE, NO_ESCAPE );
	BUILTIN_FN( "-",       builtin_sub,     TypeSet::Number(), NO_ESCAPE, NO_ESCAPE );
	BUILTIN_FN( "*",       builtin_mul,     TypeSet::Number(), NO_ESCAPE, NO_ESCAPE );
	BUILTIN_FN( "/",       builtin_div,     TypeSet::Number(), NO_ESCAPE, NO_ESCAPE );
	// ***** LOGIC
	BUILTIN_FN( "&",       builtin_and,     TypeSet::Number(), NO_ESCAPE, NO_ESCAPE );
	BUILTIN_FN( "|",       builtin_or,      TypeSet::Number(), NO_ESCAPE, NO_ESCAPE );
	// ***** REFS
	BUILTIN_FN( "ref",     builtin_ref,     TypeSet::Ref() );
	BUILTIN_FN( "get",     builtin_get,     TypeSet::all(), NO_ESCAPE );
	BUILTIN_FN( "set",     builtin_set,     TypeSet::Nil(), NO_ESCAPE, ESCAPE_GLOBAL );
	// ***** EQUALITY
	BUILTIN_FN( "=",       builtin_equal,   TypeSet::all(), NO_ESCAPE, NO_ESCAPE );
	BUILTIN_FN( "<",       builtin_lt,      TypeSet::all(), NO_ESCAPE, NO_ESCAPE );
	BUILTIN_FN( ">",       builtin_gt,      TypeSet::all(), NO_ESCAPE, NO_ESCAPE );
	BUILTIN_FN( "<=",      builtin_le,      TypeSet::all(), NO_ESCAPE, NO_ESCAPE );
	BUILTIN_FN( ">=",      builtin_ge,      TypeSet::all(), NO_ESCAPE, NO_ESCAPE );
	// ***** IO
	BUILTIN_FN( "print",   builtin_print,   TypeSet::Nil(), NO_ESCAPE );
	BUILTIN_FN( "println", builtin_println, TypeSet::Nil(), NO_ESCAPE );
}

// (define sum (lambda (a b) (if (= a 0) b (sum (- a 1) (+ 1 b)))))




























