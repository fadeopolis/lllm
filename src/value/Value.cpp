
#include "lllm/value/Value.hpp"
#include "lllm/value/ValueIO.hpp"
#include "lllm/ast/Ast.hpp"
#include "lllm/util/InternedString.hpp"

#include <cassert>
#include <cstring>
#include <cstdlib>

#include <iostream>

using namespace lllm;
using namespace lllm::value;
using namespace lllm::util;

Cons::Cons( ValuePtr car, ListPtr cdr )       : List( Type::Cons ), car( car ), cdr( cdr ) {}
Int::Int()                                    : Int( 0 ) {}
Int::Int( long value )                        : Number( Type::Int ), value( value ) {}
Real::Real()                                  : Real( 0 ) {}
Real::Real( double value )                    : Number( Type::Real ), value( value ) {}
Char::Char( char value )                      : Value( Type::Char ), value( value ) {}
String::String( CStr value )                  : Value( Type::String ), value( value ) {}
Symbol::Symbol( const InternedString& value ) : Value( Type::Symbol ), value( value ) {}
Ref::Ref()                                    : Ref( nullptr ) {}
Ref::Ref( ValuePtr value )                    : Value( Type::Ref ), value( value ) {}
Lambda::Lambda( size_t        arity,
                Lambda::Data* data,
                Lambda::FnPtr code     ) : Value( Type(size_t(Type::Lambda) + arity) ), code( code ), data( data ) {}

ValuePtr Ref::get()             const { return value; }
ValuePtr Ref::set( ValuePtr v ) const {
	ValuePtr old = value;
	value = v;
	return old;
}

bool lllm::operator==( const Value& a, const Value& b ) {
	return equal( &a, &b );
}
bool lllm::operator!=( const Value& a, const Value& b ) {
	return !(a == b);
}
bool value::equal( ValuePtr a, ValuePtr b ) {
	struct V1 final {
		bool visit( NilPtr     , NilPtr      ) const { return true; }
		bool visit( ConsPtr   a, ConsPtr   b ) const { return equal( a->car, b->car ) && equal( a->cdr, b->cdr ); }
		bool visit( IntPtr    a, IntPtr    b ) const { return a->value == b->value; }
		bool visit( RealPtr   a, IntPtr    b ) const { return a->value == b->value; }
		bool visit( IntPtr    a, RealPtr   b ) const { return a->value == b->value; }
		bool visit( RealPtr   a, RealPtr   b ) const { return a->value == b->value; }
		bool visit( CharPtr   a, CharPtr   b ) const { return a->value == b->value; }
		bool visit( StringPtr a, StringPtr b ) const { return std::strcmp( a->value, b->value ) == 0; }
		bool visit( SymbolPtr a, SymbolPtr b ) const { return a->value == b->value; }
		bool visit( RefPtr    a, RefPtr    b ) const { return a == b; }
		bool visit( LambdaPtr a, LambdaPtr b ) const { return a == b; }

		/// catch all other cases
		bool visit( ValuePtr , ValuePtr ) const { return false; }
	};
	struct V2 final {
		bool visit( NilPtr    a, ValuePtr b ) const { return value::visit<bool>( b, V1(), a ); }
		bool visit( ConsPtr   a, ValuePtr b ) const { return value::visit<bool>( b, V1(), a ); }
		bool visit( IntPtr    a, ValuePtr b ) const { return value::visit<bool>( b, V1(), a ); }
		bool visit( RealPtr   a, ValuePtr b ) const { return value::visit<bool>( b, V1(), a ); }
		bool visit( CharPtr   a, ValuePtr b ) const { return value::visit<bool>( b, V1(), a ); }
		bool visit( StringPtr a, ValuePtr b ) const { return value::visit<bool>( b, V1(), a ); }
		bool visit( SymbolPtr a, ValuePtr b ) const { return value::visit<bool>( b, V1(), a ); }
		bool visit( RefPtr    a, ValuePtr b ) const { return value::visit<bool>( b, V1(), a ); }
		bool visit( LambdaPtr a, ValuePtr b ) const { return value::visit<bool>( b, V1(), a ); }
	};

	bool eq = visit<bool>( a, V2(), b );

	return eq;
}

bool Value::isList( ValuePtr val ) {
	return typeOf( val ) <= Type::Cons;
}
bool Value::isLambda( ValuePtr val ) {
	return typeOf( val ) >= Type::Lambda;
}

#define LLLM_VISIT( TYPE )
#define LLLM_VISIT_CONCRETE( TYPE )                                                     \
	TYPE##Ptr Value::as##TYPE( ValuePtr val ) {                                         \
		return (typeOf( val ) == Type::TYPE) ? static_cast<TYPE##Ptr>( val ) : nullptr; \
	}
#include "lllm/value/Value.inc"

NumberPtr Value::asNumber( ValuePtr val ) {
	switch ( typeOf( val ) ) {
		case Type::Int:  return static_cast<IntPtr> ( val );
		case Type::Real: return static_cast<RealPtr>( val );
		default:          return nullptr;
	}
}
LambdaPtr Value::asLambda( ValuePtr val ) {
	if ( typeOf( val ) < Type::Lambda ) return nullptr;

	return static_cast<LambdaPtr>( val );
}
LambdaPtr Value::asLambda( ValuePtr val, size_t arity ) {
	if ( typeOf( val ) != Type(size_t(Type::Lambda) + arity) ) return nullptr;

	return static_cast<LambdaPtr>( val );
}

Type value::typeOf( ValuePtr v ) {
	return v ? v->type : Type::Nil;
}

size_t Lambda::arity() const { return size_t(type) - size_t(Type::Lambda); }

namespace lllm { namespace value {
	static IntPtr INTS[3];
	#define NUM_INTS     (sizeof(INTS)/sizeof(IntPtr))
	#define MIN_INT (0 - (long)((sizeof(INTS)/sizeof(IntPtr))/2))
	#define MAX_INT (0 + (long)((sizeof(INTS)/sizeof(IntPtr))/2))
}}

unsigned long long value::cacheHits   = 0;
unsigned long long value::cacheMisses = 0;

NilPtr    value::nil   = nullptr;
ValuePtr  value::True() { return number(1); }
ValuePtr  value::False = nullptr;
ConsPtr   value::cons( ValuePtr car, ListPtr cdr )           { return new Cons( car, cdr );  }
IntPtr    value::number( int    value )                      { return number( (long)value ); }
IntPtr    value::number( long   value )                      {
	if ( (MIN_INT <= value) && (value <= MAX_INT) ) {
		IntPtr i = INTS[value + MAX_INT];

		if ( !i ) { i = INTS[value + MAX_INT] = new Int( value ); }

		return i;
	}

	return new Int( value );
}
RealPtr   value::number( float  value )                      { return new Real( value );     }
RealPtr   value::number( double value )                      { return new Real( value );     }
CharPtr   value::character( char value )                     { return new Char( value );     }
StringPtr value::string( util::CStr value )                  { return new String( value );   }
SymbolPtr value::symbol( const util::InternedString& value ) { return new Symbol( value );   }
RefPtr    value::ref()                                       { return ref( nullptr );        }
RefPtr    value::ref( ValuePtr value )                       { return new Ref( value );      }

Lambda* Lambda::alloc( ast::LambdaPtr ast ) {
	return alloc( ast, nullptr );
}
Lambda* Lambda::alloc( ast::LambdaPtr ast, Lambda::FnPtr code ) {
	Lambda::DataPtr data    = ast->data;
	size_t          arity   = ast->arity();
	size_t          envSize = ast->envSize();

	if ( code ) {
		assert(!data->code);
		data->code = code;
	}

	void* memory = new char[sizeof(Lambda) + envSize * sizeof(ValuePtr)];

	value::Lambda* closure = new (memory) Lambda( arity, data, code );

	for ( size_t i = 0; i < envSize; ++i ) {
		closure->env[i] = nullptr;
	}

	return closure;
}
Lambda* Lambda::alloc( size_t arity, size_t envSize, FnPtr code ) {
	Lambda::Data* data = new Data( nullptr );

	void* memory = new char[sizeof(Lambda) + envSize * sizeof(ValuePtr)];

	value::Lambda* clojure = new (memory) Lambda( arity, data, code );

	for ( size_t i = 0; i < envSize; ++i ) {
		clojure->env[i] = nullptr;
	}

	return clojure;
}
