
#include "lllm/analyzer/AstIO.hpp"

#include <iostream>

using namespace lllm;

std::ostream& analyzer::operator<<( std::ostream& os, ConstAstPtr ast ) {
	struct Visitor {
		void visit( ConstNilPtr      ast, std::ostream& os ) const { os << "()"; }
		void visit( ConstIntPtr      ast, std::ostream& os ) const { os << ast->value; }
		void visit( ConstRealPtr     ast, std::ostream& os ) const { os << ast->value; }
		void visit( ConstCharPtr     ast, std::ostream& os ) const {
			switch ( ast->value ) {
				case '\t': os << "\\tab";     break;
				case '\n': os << "\\newline"; break;
				default:   os << '\\' << ast->value; break;
			}
		}
		void visit( ConstStringPtr   ast, std::ostream& os ) const { os << '"' << ast->value << '"'; }
		void visit( ConstVariablePtr ast, std::ostream& os ) const { os << ast->name;  }
		void visit( ConstQuotePtr    ast, std::ostream& os ) const { os << "'" << ast->value; }
		void visit( ConstIfPtr       ast, std::ostream& os ) const { 	
			os << "(if " << ast->test << " " << ast->thenBranch << " " << ast->elseBranch << ")"; 
		}
		void visit( ConstDoPtr       ast, std::ostream& os ) const {
			os << "(do " << ast->exprs.front();
			for ( auto it = ast->exprs.begin()++, end = ast->exprs.end(); it != end; ++it ) {
				os << ' ' << (*it);
			}
			os << ")";
		}
		void visit( ConstDefinePtr   ast, std::ostream& os ) const {
			os << "(define " << ast->name << " " << ast->var << ")";
		}
		void visit( ConstLetPtr      ast, std::ostream& os ) const {
			os << "(let (";

			visit_( ast->bindings.front(), os );
			for ( auto it = ++(ast->bindings.begin()), end = ast->bindings.end(); it != end; ++it ) {
				os << " ";
				visit_( *it, os );
			}

			os << ") " << ast->expr;
			os << ")";
		}
		void visit( ConstLambdaPtr ast, std::ostream& os ) const {
			os << "(lambda (";

			os << ast->parameters.front();
			for ( auto it = ++(ast->parameters.begin()), end = ast->parameters.end(); it != end; ++it ) {
				os << " " << *it;
			}

			os << ") " << ast->expr;
			os << ")";
		}
		void visit( ConstApplicationPtr ast, std::ostream& os ) const {
			os << '(';
			os << ast->fun;
			for ( auto it = ast->args.begin(), end = ast->args.end(); it != end; ++it ) {
				os << ' ' << *it;
			}
			os << ')';
		}
		void visit( ConstBuiltinPtr ast, std::ostream& os ) const {

		}

		void visit_( ConstVariablePtr binding, std::ostream& os ) const {
			os << '(' << binding->name << ' ' << binding->value << ')';
		}
	};

	ast->visit<void,Visitor,std::ostream&>( Visitor(), os );

	return os;
}

