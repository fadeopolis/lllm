
#include "lllm/sexpr/SexprIO.hpp"
#include "lllm/util/util_io.hpp"

#include <iostream>

using namespace std;
using namespace lllm;
using namespace lllm::sexpr;

std::ostream& lllm::operator<<( std::ostream& os, SexprPtr expr ) {
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

			auto it  = sexpr::begin( expr);
			auto end = sexpr::end( expr );

			if ( it != end ) {
				os << *it;
				++it;
			}

			visit( it, end, os );

			os << ')';
		}
		void visit( SexprIterator it, const SexprIterator end, std::ostream& os ) const {
			if ( it != end ) {
				os << ' ' << *it;
				visit( ++it, end, os );
			}
		}
	};

	expr->visit<void,Visitor,std::ostream&>( Visitor(), os );
	return os;
}

