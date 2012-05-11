
#include "lllm/sexpr/Sexpr.hpp"

#include <cstring>

using namespace lllm;
using namespace lllm::sexpr;
using namespace lllm::util;

#define LLLM_VISITOR( TYPE, VALUE ) \
	TYPE::TYPE( const SourceLocation& loc, const VALUE& value ) : Sexpr( Type::TYPE, loc ), value( value ) {}
#include "lllm/sexpr/Sexpr_concrete.inc"

#define LLLM_VISITOR( TYPE, ... )                  \
	bool      Sexpr::is##TYPE() const {            \
		return type == Type::TYPE;                 \
	}                                              \
	TYPE##Ptr Sexpr::as##TYPE() const {            \
		if ( type == Type::TYPE ) {                \
			return static_cast<TYPE##Ptr>( this ); \
		} else {                                   \
			return nullptr;                        \
		}                                          \
	}
#include "lllm/sexpr/Sexpr_concrete.inc"


size_t        sexpr::length( ListPtr l )         { return l->value.size();  }
SexprIterator sexpr::begin( ListPtr l )          { return l->value.begin(); }
SexprIterator sexpr::end( ListPtr l)             { return l->value.end();   }
SexprPtr      sexpr::at( ListPtr l, size_t idx ) { return l->value[idx];    }


bool lllm::operator!=( const sexpr::Sexpr& a, const sexpr::Sexpr& b ) {
	return !(a == b);
}
bool lllm::operator==( const sexpr::Sexpr& a, const sexpr::Sexpr& b ) {
	return equal( &a, &b );
}
bool sexpr::equal( SexprPtr a, SexprPtr b ) {
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
			if ( length( a ) != length( b ) ) return false;

			auto aIt = begin( a );
			auto bIt = begin( b );

			for ( auto end = sexpr::end( a ); aIt != end; ++aIt, ++bIt ) {
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

static List NIL( SourceLocation("*test*"), SexprVector() );

SexprPtr  sexpr::nil = &NIL;
IntPtr    sexpr::number( int    value )                { return new Int( SourceLocation("*test*"), value );     }
IntPtr    sexpr::number( long   value )                { return new Int( SourceLocation("*test*"), value );     }
RealPtr   sexpr::number( float  value )                { return new Real( SourceLocation("*test*"), value );    }
RealPtr   sexpr::number( double value )                { return new Real( SourceLocation("*test*"), value );    }
CharPtr   sexpr::character( char value )               { return new Char( SourceLocation("*test*"), value );    }
StringPtr sexpr::string( CStr value )                  { return new String( SourceLocation("*test*"), value );  }
SymbolPtr sexpr::symbol( const InternedString& value ) { return new Symbol( SourceLocation("*test*"), value );  }
ListPtr   sexpr::list()                                { return new List( SourceLocation("*test*"), SexprVector() ); }
ListPtr   sexpr::cons( SexprPtr car, ListPtr cdr )     {
	std::vector<SexprPtr> exprs;

	exprs.push_back( car );
	for ( auto it = begin( cdr ), end = sexpr::end( cdr ); it != end; ++it ) {
		exprs.push_back( *it );
	}

	return new List( SourceLocation("*test*"), exprs );
}

