
#include "lllm/reader/Sexpr.hpp"
#include "lllm/reader/SexprIO.hpp"

#include <cstring>
#include <iostream>

using namespace lllm;
using namespace lllm::reader;

Sexpr::Sexpr( Sexpr::Type t, const SourceLocation& sl )                    : type( t ), location( sl )                 {}
Int::Int( const SourceLocation& sl, long value )                           : Sexpr( Type::Int, sl ), value( value )    {}
Real::Real( const SourceLocation& sl, double value )                       : Sexpr( Type::Real, sl ), value( value )   {}
Char::Char( const SourceLocation& sl, char value )                         : Sexpr( Type::Char, sl ), value( value )   {}
String::String( const SourceLocation& sl, CStr value )                     : Sexpr( Type::String, sl ), value( value ) {}
Symbol::Symbol( const SourceLocation& sl, CStr value )                     : Sexpr( Type::Symbol, sl ), value( value ) {}
List::List( const SourceLocation& sl )                                     : Sexpr( Type::List, sl ), exprs()          {}
List::List( const SourceLocation& sl, const std::vector<SexprPtr>& exprs ) : Sexpr( Type::List, sl ), exprs( exprs )   {}

#define LLLM_VISITOR( TYPE )              \
	bool Sexpr::is##TYPE() const {        \
		return type == Sexpr::Type::TYPE; \
	}
#include "lllm/reader/Sexpr.inc"

#define LLLM_VISITOR( TYPE )                       \
	TYPE##Ptr Sexpr::as##TYPE() const {            \
		if ( type == Sexpr::Type::TYPE ) {         \
			return static_cast<TYPE##Ptr>( this ); \
		} else {                                   \
			return nullptr;                        \
		}                                          \
	}
#include "lllm/reader/Sexpr.inc"

SexprPtr List::operator[]( size_t idx ) const { return exprs[idx]; }
SexprPtr List::at( size_t idx )         const { return exprs[idx]; }

List::iterator List::begin() const { return exprs.begin(); }
List::iterator List::end()   const { return exprs.end();   }

size_t List::length() const { return exprs.size(); }

bool reader::operator!=( SexprRef a, SexprRef b ) {
	return !(a == b);
}
bool reader::operator==( SexprRef a, SexprRef b ) {
	return equal( &a, &b );
}
bool reader::equal( SexprPtr a, SexprPtr b ) {
	using namespace std;
//	std::cout << a << "=?=" << b << std::endl;

	struct V1 final {
		bool visit( IntPtr    a, SexprPtr b ) const { return false; }
		bool visit( RealPtr   a, SexprPtr b ) const { return false; }
		bool visit( CharPtr   a, SexprPtr b ) const { return false; }
		bool visit( StringPtr a, SexprPtr b ) const { return false; }
		bool visit( SymbolPtr a, SexprPtr b ) const { return false; }
		bool visit( ListPtr   a, SexprPtr b ) const { return false; }

		bool visit( IntPtr    a, IntPtr    b ) const { return a->value == b->value; }
		bool visit( RealPtr   a, IntPtr    b ) const { return a->value == b->value; }
		bool visit( IntPtr    a, RealPtr   b ) const { return a->value == b->value; }
		bool visit( RealPtr   a, RealPtr   b ) const { return a->value == b->value; }
		bool visit( CharPtr   a, CharPtr   b ) const { return a->value == b->value; }
		bool visit( StringPtr a, StringPtr b ) const { return std::strcmp( a->value, b->value ) == 0; }
		bool visit( SymbolPtr a, SymbolPtr b ) const { return a->value == b->value; }
		bool visit( ListPtr   a, ListPtr   b ) const {
			if ( a->length() != b->length() ) return false;

			auto aIt = a->begin();
			auto bIt = b->begin();

			for ( auto end = a->end(); aIt != end; ++aIt, ++bIt ) {
				if ( **aIt != **bIt ) return false;
			}
			return true;
		}
	};
	struct V2 final {
		bool visit( IntPtr    a, SexprPtr b ) const { return b->visit<bool>( V1(), a ); }
		bool visit( RealPtr   a, SexprPtr b ) const { return b->visit<bool>( V1(), a ); }
		bool visit( CharPtr   a, SexprPtr b ) const { return b->visit<bool>( V1(), a ); }
		bool visit( StringPtr a, SexprPtr b ) const { return b->visit<bool>( V1(), a ); }
		bool visit( SymbolPtr a, SexprPtr b ) const { return b->visit<bool>( V1(), a ); }
		bool visit( ListPtr   a, SexprPtr b ) const { return b->visit<bool>( V1(), a ); }
	};

	return a->visit<bool>( V2(), b );
}

static inline SourceLocation loc() {
	return SourceLocation("*test*");
}

SexprPtr  reader::nil()                             { return list();                       }
IntPtr    reader::number( int    value )            { return new Int( loc(), value );     }
IntPtr    reader::number( long   value )            { return new Int( loc(), value );     }
RealPtr   reader::number( float  value )            { return new Real( loc(), value );    }
RealPtr   reader::number( double value )            { return new Real( loc(), value );    }
CharPtr   reader::character( char value )           { return new Char( loc(), value );    }
StringPtr reader::string( CStr value )              { return new String( loc(), value );  }
SymbolPtr reader::symbol( CStr value )              { return new Symbol( loc(), value );  }
ListPtr   reader::list()                            { return new List( loc() );           }
ListPtr   reader::cons( SexprPtr car, ListPtr cdr ) {
	std::vector<SexprPtr> exprs;
//	exprs.resize( cdr->length() + 1 );

	exprs.push_back( car );
	for ( auto it = cdr->begin(), end = cdr->end(); it != end; ++it ) {
		exprs.push_back( *it );
	}

	return new List( loc(), exprs );
}

