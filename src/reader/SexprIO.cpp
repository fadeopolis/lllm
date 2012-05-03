
#include "lllm/reader/SexprIO.hpp"

#include <iostream>

using namespace std;
using namespace lllm;
using namespace lllm::reader;

std::ostream& reader::operator<<( std::ostream& os, SexprRef expr ) {
	return os << (&expr);
}
std::ostream& reader::operator<<( std::ostream& os, SexprPtr expr ) {
	struct Visitor final {
		void visit( IntPtr expr, std::ostream& os ) const {
			os << expr->value;
		}
		void visit( RealPtr expr, std::ostream& os ) const {
			os << expr->value;
		}
		void visit( CharPtr expr, std::ostream& os ) const {
			switch ( expr->value ) {
				case '\t': os << "\\tab";            break;
				case '\n': os << "\\newline";        break;
				default:   os << '\\' << expr->value; break;
			}
		}
		void visit( StringPtr expr, std::ostream& os ) const {
			os << '"' << expr->value << '"';
		}
		void visit( SymbolPtr expr, std::ostream& os ) const {
			os << expr->value;
		}
		void visit( ListPtr expr, std::ostream& os ) const {
			os << '(';

			auto it  = expr->begin();
			auto end = expr->end();

			if ( it != end ) {
				os << *it;
				++it;
			}

			visit( it, end, os );

			os << ')';
		}
		void visit( List::iterator it, const List::iterator end, std::ostream& os ) const {
			if ( it != end ) {
				os << ' ' << *it;
				visit( ++it, end, os );
			}
		}
	};

	expr->visit<void,Visitor,std::ostream&>( Visitor(), os );
	return os;
}

