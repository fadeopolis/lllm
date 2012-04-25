
#include "reader/ParseTree.hpp"

#include <cstring>
#include <iostream>

using namespace lllm;

ParseTree::ParseTree( const SourceLocation& sl ) : location( sl ) {}
NumberTree::NumberTree( const SourceLocation& sl ) : ParseTree( sl ) {}
IntTree::IntTree( const SourceLocation& sl, long value ) : NumberTree( sl ), value( value ) {}
RealTree::RealTree( const SourceLocation& sl, double value ) : NumberTree( sl ), value( value ) {}
CharTree::CharTree( const SourceLocation& sl, char value ) : ParseTree( sl ), value( value ) {}
StringTree::StringTree( const SourceLocation& sl, CStr value ) : ParseTree( sl ), value( value ) {}
SymbolTree::SymbolTree( const SourceLocation& sl, CStr value ) : ParseTree( sl ), value( value ) {}
ListTree::ListTree( const SourceLocation& sl, ParseTree* car, ListTree* cdr ) : ParseTree( sl ), car( car ), cdr( cdr ) {}

NumberTree::~NumberTree() {}

void IntTree::getVisitedBy( ParseTreeVisitor& v )    { v.visit( this ); }
void RealTree::getVisitedBy( ParseTreeVisitor& v )   { v.visit( this ); }
void CharTree::getVisitedBy( ParseTreeVisitor& v )   { v.visit( this ); }
void StringTree::getVisitedBy( ParseTreeVisitor& v ) { v.visit( this ); }
void SymbolTree::getVisitedBy( ParseTreeVisitor& v ) { v.visit( this ); }
void ListTree::getVisitedBy( ParseTreeVisitor& v )   { v.visit( this ); }

void IntTree::printTo( std::ostream& os )    const { os << value; }
void RealTree::printTo( std::ostream& os )   const { os << value; }
void CharTree::printTo( std::ostream& os )   const { os << value; }
void StringTree::printTo( std::ostream& os ) const { os << value; }
void SymbolTree::printTo( std::ostream& os ) const { os << value; }
void ListTree::printTo( std::ostream& os )   const {
	os << '(';
	os << car;

	for ( const ListTree* list = cdr; list; list = list->cdr ) {
		os << ' ' << list->car;
	}

	os << ')';
}

std::ostream& lllm::operator<<( std::ostream& os, const ParseTree& p ) {
	p.printTo( os );
	return os;
}
std::ostream& lllm::operator<<( std::ostream& os, const ParseTree* p ) {
	if ( p ) {
		p->printTo( os );
		return os;
	} else {
		return os << "()";
	}
}

bool ParseTree::equal( const ParseTree* a, const ParseTree* b ) {
	if ( a == b ) return true;

	if ( const IntTree* aI = dynamic_cast<const IntTree*>( a ) ) {
		if ( const IntTree* bI = dynamic_cast<const IntTree*>( b ) ) {
			return aI->value == bI->value;
		}
		if ( const RealTree* bI = dynamic_cast<const RealTree*>( b ) ) {
			return aI->value == bI->value;
		}
		return false;
	}
	if ( const RealTree* aI = dynamic_cast<const RealTree*>( a ) ) {
		if ( const IntTree* bI = dynamic_cast<const IntTree*>( b ) ) {
			return aI->value == bI->value;
		}
		if ( const RealTree* bI = dynamic_cast<const RealTree*>( b ) ) {
			return aI->value == bI->value;
		}
		return false;
	}
	if ( const CharTree* aI = dynamic_cast<const CharTree*>( a ) ) {
		if ( const CharTree* bI = dynamic_cast<const CharTree*>( b ) ) {
			return aI->value == bI->value;
		}
		return false;
	}
	if ( const StringTree* aI = dynamic_cast<const StringTree*>( a ) ) {
		if ( const StringTree* bI = dynamic_cast<const StringTree*>( b ) ) {
			return std::strcmp( aI->value, bI->value ) == 0;
		}
		return false;
	}
	if ( const SymbolTree* aI = dynamic_cast<const SymbolTree*>( a ) ) {
		if ( const SymbolTree* bI = dynamic_cast<const SymbolTree*>( b ) ) {
			return std::strcmp( aI->value, bI->value ) == 0;
		}
		return false;
	}
	if ( const ListTree* aI = dynamic_cast<const ListTree*>( a ) ) {
		if ( const ListTree* bI = dynamic_cast<const ListTree*>( b ) ) {
			return equal( aI->car, bI->car ) && equal( aI->cdr, bI->cdr );
		}
		return false;
	}

	return false;
}

static inline SourceLocation loc() {
	return SourceLocation("*test*");
}

ParseTree* ParseTree::number( long value )                  { return new IntTree( loc(), value );     }
ParseTree* ParseTree::number( double value )                { return new RealTree( loc(), value );    }
ParseTree* ParseTree::character( char value )               { return new CharTree( loc(), value );    }
ParseTree* ParseTree::string( CStr value )                  { return new StringTree( loc(), value );  }
ParseTree* ParseTree::symbol( CStr value )                  { return new SymbolTree( loc(), value );  }
ParseTree* ParseTree::list( ParseTree* car, ListTree* cdr ) { return new ListTree( loc(), car, cdr ); }

