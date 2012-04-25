
#include "ValueIO.hpp"
#include "ValueVisitor.hpp"

#include <iostream>

using namespace lllm;
using namespace lllm::val;

std::ostream& lllm::operator<<( std::ostream& os, Type t ) {
	switch ( t ) {
		#define CASE( TYPE ) case Type::TYPE: os << #TYPE; break;
		CASE( Int )
		CASE( Real )
		CASE( Char )
		CASE( String )
		CASE( Symbol )
		CASE( Cons )
		CASE( Nil )
		CASE( Lambda )
		CASE( Thunk )
		CASE( Ref )
		#undef  CASE
	}

	return os;
}
std::ostream& lllm::operator<<( std::ostream& os, const Value& v ) {
	return os << &v;
}
std::ostream& lllm::operator<<( std::ostream& os, ValuePtr val ) {
	struct Visitor final {
		static void visit( IntPtr    v, std::ostream& os ) { os << v->value; }
		static void visit( RealPtr   v, std::ostream& os ) { os << v->value; }
		static void visit( CharPtr   v, std::ostream& os ) { os << v->value; }
		static void visit( StringPtr v, std::ostream& os ) { os << '"' << v->value << '"'; }
		static void visit( SymbolPtr v, std::ostream& os ) { os << v->value; }
		static void visit( ConsPtr   v, std::ostream& os ) {
			os << '(';

			os << car( v );

			while ( (v = castOrNil<Cons>( v->cdr ) ) ) {
				os << ' ' << v->car;
			}

			os << ')';
		}
		static void visit( NilPtr    v, std::ostream& os ) { os << "nil";      }
		static void visit( LambdaPtr v, std::ostream& os ) { os << "<lambda " << v->parameters << " " << v->body << ">"; }
		static void visit( ThunkPtr  v, std::ostream& os ) { os << "<thunk>";  }
		static void visit( RefPtr    v, std::ostream& os ) { os << "<ref " << get( v ) << ">";  }
	};

	visit<Visitor,void,std::ostream&>( val, os );

	return os;
}
