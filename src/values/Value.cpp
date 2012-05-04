
#include "lllm/values/Value.hpp"
#include "lllm/util/InternedString.hpp"

#include <cstring>
#include <cstdlib>

//#include <iostream>
//#include "lllm/values/ValueIO.hpp"

using namespace lllm;
using namespace lllm::value;
using namespace lllm::util;

void* Value::operator new( size_t size ) {
	return std::malloc( size );
}
void* Value::operator new( size_t size, void* mem ) {
	return mem;
}

Cons::Cons( ValuePtr car, ListPtr cdr ) : List( Value::Cons ), car( car ), cdr( cdr ) {}
Int::Int()                              : Int( 0 ) {}
Int::Int( long value )                  : Number( Value::Int ), value( value ) {}
Real::Real()                            : Real( 0 ) {}
Real::Real( double value )              : Number( Value::Real ), value( value ) {}
Char::Char( char value )                : Value( Value::Char ), value( value ) {}
String::String( CStr value )            : Value( Value::String ), value( value ) {}
Symbol::Symbol( CStr value )            : Value( Value::Symbol ), value( InternedString::intern( value ) ) {}
Ref::Ref()                              : Ref( nullptr ) {}
Ref::Ref( ValuePtr value )              : Value( Value::Ref ), value( value ) {}
Lambda::Lambda( size_t arity )          : Value( Value::Type(Value::Lambda + arity) ) {}

ValuePtr Ref::get()             const { return value; }
ValuePtr Ref::set( ValuePtr v ) const {
	ValuePtr old = value;
	value = v;
	return old;
}

bool value::operator==( const Value& a, const Value& b ) {
	return equal( &a, &b );
}
bool value::operator!=( const Value& a, const Value& b ) {
	return !(a == b);
}
bool value::equal( ValuePtr a, ValuePtr b ) {
//	using namespace std;
//	std::cout << a << "=?=" << b << std::endl;

	struct V1 final {
		bool visit( ValuePtr  a, ValuePtr b ) const { return false; }

		bool visit( NilPtr    a, NilPtr    b ) const { return true; }
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

	return visit<bool>( a, V2(), b );
}

bool Value::isList( ValuePtr val ) {
	return typeOf( val ) <= Value::Cons;
}

#define LLLM_VISITOR( TYPE ) 																\
	TYPE##Ptr Value::as##TYPE( ValuePtr val ) { 											\
		return (typeOf( val ) == Value::TYPE) ? static_cast<TYPE##Ptr>( val ) : nullptr;	\
	}
#include "lllm/values/Value_for_impl.inc"
NumberPtr Value::asNumber( ValuePtr val ) {
	switch ( typeOf( val ) ) {
		case Value::Int:  return static_cast<IntPtr> ( val );
		case Value::Real: return static_cast<RealPtr>( val );
		default:          return nullptr;
	}
}
LambdaPtr Value::asLambda( ValuePtr val, size_t arity ) {
	if ( typeOf( val ) != (Value::Lambda + arity) ) return nullptr;

	return static_cast<LambdaPtr>( val );
}

Value::Type value::typeOf( ValuePtr v ) {
	return v ? v->type : Value::Nil;
}

size_t Lambda::arity() const { return type - Value::Lambda; }

NilPtr    value::nil()                             { return nullptr;               }
ConsPtr   value::cons( ValuePtr car, ListPtr cdr ) { return new Cons( car, cdr );  }
IntPtr    value::number( int    value )            { return new Int( value );      }
IntPtr    value::number( long   value )            { return new Int( value );      }
RealPtr   value::number( float  value )            { return new Real( value );     }
RealPtr   value::number( double value )            { return new Real( value );     }
CharPtr   value::character( char value )           { return new Char( value );     }
StringPtr value::string( CStr value )              { return new String( value );   }
SymbolPtr value::symbol( CStr value )              { return new Symbol( value );   }
RefPtr    value::ref()                             { return ref( nullptr );        }
RefPtr    value::ref( ValuePtr value )             { return new Ref( value );      }

Lambda* Lambda::alloc( size_t arity, size_t envSize ) {
	envSize++;

	void* memory = new char[sizeof(Lambda) + envSize * sizeof(ValuePtr)];

	value::Lambda* clojure = new (memory) Lambda( arity );

	clojure->fun = nullptr;
	clojure->ast = nullptr;

	for ( size_t i = 0; i < envSize; ++i ) {
		clojure->env[i] = nullptr;
	}

	return clojure;
}


