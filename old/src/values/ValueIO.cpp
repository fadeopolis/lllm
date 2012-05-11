
#include "lllm/values/ValueIO.hpp"

#include <iostream>

using namespace lllm;
using namespace lllm::value;

std::ostream& value::operator<<( std::ostream& os, const Value::Type& t ) {
	switch ( t ) {
		#define LLLM_VISITOR( TYPE ) case Value::TYPE: return os << #TYPE;
		#include "lllm/values/Value_for_impl.inc"
		default: return os << "Lambda";
	}
}
std::ostream& value::operator<<( std::ostream& os, const Value& v ) {
	struct Visitor final {
		void visit( NilPtr expr, std::ostream& os ) const {
			os << "()";
		}
		void visit( ConsPtr expr, std::ostream& os ) const {
			os << '(' << expr->car;
			visit_( expr->cdr, os );
			os << ')';
		}
		void visit( IntPtr expr, std::ostream& os ) const {
			os << expr->value;
		}
		void visit( RealPtr expr, std::ostream& os ) const {
			os << expr->value;
		}
		void visit( CharPtr expr, std::ostream& os ) const {
			switch ( expr->value ) {
				case '\t': os << "\\tab";
				case '\n': os << "\\newline";
				default:   os << '\\' << expr->value;
			}
		}
		void visit( StringPtr expr, std::ostream& os ) const {
			os << '"' << expr->value << '"';
		}
		void visit( SymbolPtr expr, std::ostream& os ) const {
			os << expr->value;
		}
		void visit( RefPtr expr, std::ostream& os ) const {
			os << "<ref " << expr->get() << ">";
		}
		void visit( LambdaPtr expr, std::ostream& os ) const {
			os << "<lambda>";
		}
		void visit_( ListPtr expr, std::ostream& os ) const {
			if ( expr ) {
				ConsPtr cons = static_cast<ConsPtr>( expr );
				os << ' ' << cons->car;
				visit_( cons->cdr, os );
			}
		}
	};

	Visitor vi;
	visit<void,Visitor,std::ostream&>( &v, vi, os );

	return os;
}	
std::ostream& value::operator<<( std::ostream& os, const Value* v ) {
	if ( v ) 
		return os << (*v);
	else
		return os << "()";
}

