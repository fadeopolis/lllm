
#include "lllm/value/ValueIO.hpp"
#include "lllm/ast/AstIO.hpp"

#include <iostream>
#include <iomanip>

using namespace lllm;
using namespace lllm::value;

#define DBG( TYPE ) (void) ({ nullptr; })
//#define DBG( TYPE ) (void) ({ std::cout << "{VISITING " << #TYPE << "}" << std::flush; nullptr; })

std::ostream& lllm::operator<<( std::ostream& os, Type t ) {
	switch ( t ) {
		#define LLLM_VISITOR( TYPE ) case Type::TYPE: return os << #TYPE;
		#include "lllm/value/Value_concrete.inc"
		default: return os << "Lambda";
	}
}
std::ostream& lllm::operator<<( std::ostream& os, ValuePtr v ) {
	if ( !v ) return os << "()";

	if ( long(typeOf(v)) > (long(Type::Lambda) + 3) ) {
		os << "{?" << (long)typeOf( v ) << "?}";;
//		std::cerr << "{PRINTING " << (long)typeOf( v ) << "}";
		return os;
	}

	struct Visitor final {
		void visit( NilPtr expr, std::ostream& os ) const {
			DBG( Nil );
			os << "()";
		}
		void visit( ConsPtr expr, std::ostream& os ) const {
			DBG( Cons );
			os << '(' << expr->car;
			visit_( expr->cdr, os );
			os << ')';
		}
		void visit( IntPtr expr, std::ostream& os ) const {
			DBG( Int );
			os << expr->value;
		}
		void visit( RealPtr expr, std::ostream& os ) const {
			DBG( Real );
			os << expr->value;
		}
		void visit( CharPtr expr, std::ostream& os ) const {
			DBG( Char );
			switch ( expr->value ) {
				case '\t': os << "\\tab"; break;
				case '\n': os << "\\newline"; break;
				default:   os << '\\' << expr->value; break;
			}
		}
		void visit( StringPtr expr, std::ostream& os ) const {
			DBG( String );
			os << '"' << expr->value << '"';
		}
		void visit( SymbolPtr expr, std::ostream& os ) const {
			DBG( Symbol );
			if ( (void*)(util::CStr)expr->value )
				os << expr->value;
			else 
				os << "<EMPTY SYMBOL>";
		}
		void visit( RefPtr expr, std::ostream& os ) const {
			DBG( Ref );
			os << "<ref " << expr->get() << ">";
		}
		void visit( LambdaPtr expr, std::ostream& os ) const {
			DBG( Lambda );
	
			if ( expr->code ) 
				os << "<jitted " << expr->data->ast << ">";
			else
				os << "<" << expr->data->ast << ">";
		}
		void visit_( ListPtr expr, std::ostream& os ) const {
			if ( expr ) {
				ConsPtr cons = static_cast<ConsPtr>( expr );
				os << ' ' << cons->car;
				visit_( cons->cdr, os );
			}
		}
	};

	visit<void,Visitor,std::ostream&>( v, Visitor(), os );

	return os;
}	

