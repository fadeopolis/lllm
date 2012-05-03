
#include "lllm/values/Value.hpp"

#include <map>
#include <cstring>

//#include <iostream>
//#include "lllm/values/ValueIO.hpp"

using namespace lllm;
using namespace lllm::value;

Cons::Cons( ValuePtr car, ListPtr cdr ) : List( Value::Cons ), car( car ), cdr( cdr ) {}
Int::Int()                              : Int( 0 ) {}
Int::Int( long value )                  : Value( Value::Int ), value( value ) {}
Real::Real()                            : Real( 0 ) {}
Real::Real( double value )              : Value( Value::Real ), value( value ) {}
Char::Char( char value )                : Value( Value::Char ), value( value ) {}
String::String( CStr value )            : Value( Value::String ), value( value ) {}
Symbol::Symbol( CStr value )            : Value( Value::Symbol ), value( value ) {}
Ref::Ref()                              : Ref( nullptr ) {}
Ref::Ref( ValuePtr value )              : Value( Value::Ref ), value( value ) {}
Lambda::Lambda( size_t arity )          : Value( Value::Type(Value::Lambda + arity) ) {}

SymbolPtr Symbol::make( CStr value ) {
	struct CmpStr final {
		bool operator()(char const *a, char const *b) { 
			return std::strcmp(a, b) < 0;
		}
	};

	typedef std::map<CStr, SymbolPtr, CmpStr> InternTable;

	static InternTable intern_table;

	auto lb = intern_table.lower_bound( value );

	if ( lb != intern_table.end() && (std::strcmp( value, lb->first ) == 0) ) {
		// symbol already exists
		return lb->second;
	} else {
		// the symbol does not exist in the map
		SymbolPtr sym = new Symbol( value );

		// add it to the map using lb as a hint to insert, so it can avoid another lookup
		intern_table.insert( lb, InternTable::value_type( value, sym ) );

		return sym;
	}
}

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

#define LLLM_VISITOR( TYPE ) 																\
	TYPE##Ptr Value::as##TYPE( ValuePtr val ) { 											\
		return (typeOf( val ) == Value::TYPE) ? static_cast<TYPE##Ptr>( val ) : nullptr;	\
	}
#include "lllm/values/Value_for_impl.inc"
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
SymbolPtr value::symbol( CStr value )              { return Symbol::make( value ); }
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


