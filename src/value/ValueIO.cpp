
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
	struct V final {
		void operator()( IntPtr    v, std::ostream& os ) { os << v->value; }
		void operator()( RealPtr   v, std::ostream& os ) { os << v->value; }
		void operator()( CharPtr   v, std::ostream& os ) { os << v->value; }
		void operator()( StringPtr v, std::ostream& os ) { os << '"' << v->value << '"'; }
		void operator()( SymbolPtr v, std::ostream& os ) { os << v->value; }
		void operator()( ConsPtr   v, std::ostream& os ) {
			os << '(';

			os << car( v );

			ValuePtr tmp = cdr( v );

			while ( tmp ) {
				if ( isCons( tmp ) ) {
					ConsPtr cons = cast<Cons>( tmp );
					os << ' ' << car( cons );
					tmp = cdr( cons );
				} else {
					os << ' ' << tmp;
					break;
				}
			}

			os << ')';
		}
		void operator()( NilPtr    v, std::ostream& os ) { os << "nil";      }
		void operator()( LambdaPtr v, std::ostream& os ) { os << "<lambda>"; }
		void operator()( ThunkPtr  v, std::ostream& os ) { os << "<thunk>";  }
		void operator()( RefPtr    v, std::ostream& os ) { os << "<ref " << v->value << ">";  }
	};

	V v;
	visit<V,void,std::ostream&>( val, v, os );

	return os;
}
