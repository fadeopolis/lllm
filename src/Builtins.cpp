
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
	(void) fn;

	if ( Value::isList( cdr ) ) {
		return cons( car, static_cast<ListPtr>( cdr ) );
	} else {
		LLLM_FAIL( "builtin function 'cons' expects a list as second argument, not a " << cdr );
	}
}
static value::ValuePtr builtin_car( LambdaPtr fn, ValuePtr v ) {
	(void) fn;

	if ( ConsPtr cons = Value::asCons( v ) ) {
		return cons->car;
	} else {
		LLLM_FAIL( "builtin function 'car' expects a cons as first argument, not a " << v );
	}
}
static ValuePtr builtin_cdr( LambdaPtr fn, ValuePtr v ) {
	(void) fn;

	if ( ConsPtr cons = Value::asCons( v ) ) {
		return cons->cdr;
	} else {
		LLLM_FAIL( "builtin function 'cdr' expects a cons as first argument, not a " << v );
	}
}

//***** ARITHMETIC *****************************************************************************************************

#define BUILTIN_BINARY_ARITH( NAME, OP ) 																					\
	static ValuePtr builtin_ ## NAME( LambdaPtr fn, ValuePtr A, ValuePtr B ) {												\
		(void) fn;																											\
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
		}																													\
	}

BUILTIN_BINARY_ARITH(add, +)
BUILTIN_BINARY_ARITH(sub, -)
BUILTIN_BINARY_ARITH(mul, *)
BUILTIN_BINARY_ARITH(div, /)

//***** LOGIC **********************************************************************************************************

static ValuePtr builtin_and( LambdaPtr fn, ValuePtr a, ValuePtr b ) { (void) fn; return (a && b) ? TRUE : nullptr; }
static ValuePtr builtin_or ( LambdaPtr fn, ValuePtr a, ValuePtr b ) { (void) fn; return (a || b) ? TRUE : nullptr; }

//***** RELATIONS ******************************************************************************************************
static ValuePtr builtin_equal( LambdaPtr fn, ValuePtr a, ValuePtr b ) {
	(void) fn;
	return equal( a, b ) ? TRUE : nullptr;
}
#define BUILTIN_BINARY_CMP( NAME, OP )                                                                                                   \
	static ValuePtr builtin_ ## NAME( LambdaPtr fn, ValuePtr A, ValuePtr B ) {                                                           \
		(void) fn;                                                                                                                       \
		switch ( typeOf( A ) ) {                                                                                                         \
			case Type::Int:                                                                                                              \
				switch ( typeOf( B ) ) {                                                                                                 \
					case Type::Int:  return ((static_cast<IntPtr>( A )->value) OP (static_cast<IntPtr> ( B )->value)) ? TRUE : nullptr;  \
					case Type::Real: return ((static_cast<IntPtr>( A )->value) OP (static_cast<RealPtr>( B )->value)) ? TRUE : nullptr;  \
					default: LLLM_FAIL( "builtin function '" #OP "' expects a number as second argument, not a " << B );                 \
				}                                                                                                                        \
			case Type::Real:                                                                                                             \
				switch ( typeOf( B ) ) {                                                                                                 \
					case Type::Int:  return ((static_cast<RealPtr>( A )->value) OP (static_cast<IntPtr> ( B )->value)) ? TRUE : nullptr; \
					case Type::Real: return ((static_cast<RealPtr>( A )->value) OP (static_cast<RealPtr>( B )->value)) ? TRUE : nullptr; \
					default: LLLM_FAIL( "builtin function '" #OP "' expects a number as second argument, not a " << B );                 \
				}                                                                                                                        \
			default: LLLM_FAIL( "builtin function '" #OP "' expects a number as first argument, not a " << A );                          \
		}                                                                                                                                \
	}

BUILTIN_BINARY_CMP(lt, <)
BUILTIN_BINARY_CMP(gt, >)
BUILTIN_BINARY_CMP(le, <=)
BUILTIN_BINARY_CMP(ge, >=)

//***** REFS ***********************************************************************************************************

static ValuePtr builtin_ref( LambdaPtr fn ) {
	(void) fn;

	return value::ref();
}
static ValuePtr builtin_get( LambdaPtr fn, ValuePtr ref ) {
	(void) fn;

	if ( RefPtr r = Value::asRef( ref ) ) {
		return r->get();
	} else {
		LLLM_FAIL( "builtin function 'get' expects a ref as first argument, not a " << ref );
	}
}
static ValuePtr builtin_set( LambdaPtr fn, ValuePtr ref, ValuePtr v ) {
	(void) fn;

	if ( RefPtr r = Value::asRef( ref ) ) {
		return r->set( v );
	} else {
		LLLM_FAIL( "builtin function 'set' expects a ref as first argument, not a " << ref );
	}
}

//***** IO *************************************************************************************************************
static ValuePtr builtin_print( LambdaPtr fn, ValuePtr v ) {
	(void) fn;

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

inline ast::LambdaPtr makeBuiltinFn( CStr name, TypeSet returnT, std::initializer_list<EscapeStatus> ps ) {
	(void) returnT;

	std::vector<ast::VariablePtr> params;

	for ( size_t i = 0; i < ps.size(); i++ ) {
		auto param = ast::Variable::makeParameter( builtin_location, "arg" );

		params.push_back( param );
	}

	auto ast = new ast::Lambda( builtin_location, name, params, std::vector<ast::VariablePtr>(), nullptr );

	for ( size_t i = 0; i < ps.size(); i++ ) {
		ast->paramEscape( ast->params_begin() + i, ps.begin()[i] );
	}

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
		auto ast = makeBuiltinFn( NAME, RETURN, { ESCAPES } );                 \
		auto var = ast::Variable::makeGlobal( builtin_location, (NAME), ast ); \
		auto fun = Lambda::alloc( ast, (Lambda::FnPtr) (void*) FN );           \
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
