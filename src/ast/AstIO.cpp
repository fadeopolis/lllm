
#include "lllm/ast/AstIO.hpp"
#include "lllm/value/ValueIO.hpp"
#include "lllm/util/util_io.hpp"

#include <iostream>
#include <cstring>

using namespace lllm;
using namespace lllm::ast;

#if LLLM_DBG_LVL > 5
#	include <iomanip>
#	define DBG_BEGIN( TYPE ) (void) ({ std::cout << "{VISITING " << #TYPE << "}" << std::flush; nullptr; })
#	define DBG_END( TYPE )   (void) ({ std::cout << "{VISITED  " << #TYPE << "}" << std::flush; nullptr; })
#else
#	define DBG_BEGIN( TYPE ) (void) ({ nullptr; })
#	define DBG_END( TYPE )   (void) ({ nullptr; })
#endif

std::ostream& lllm::operator<<( std::ostream& os, ConstAstPtr ast ) {
	if ( !ast ) {
		return os << "<*builtin*>";
	}
	struct Visitor {
		void visit( ConstNilPtr      ast, std::ostream& os ) const { 
			DBG_BEGIN( Nil ); 
			os << "()";
			DBG_END( Nil ); 
		}
		void visit( ConstIntPtr      ast, std::ostream& os ) const { 
			DBG_BEGIN( Int ); 
			os << ast->value; 
			DBG_END( Int ); 
		}
		void visit( ConstRealPtr     ast, std::ostream& os ) const { 
			DBG_BEGIN( Real );
			os << ast->value; 
			DBG_END( Real );
		}
		void visit( ConstCharPtr     ast, std::ostream& os ) const { 
			DBG_BEGIN( Char );
			switch ( ast->value ) {
				case '\t': os << "\\tab";     break;
				case '\n': os << "\\newline"; break;
				default:   os << '\\' << ast->value; break;
			}
			DBG_END( Char );
		}
		void visit( ConstStringPtr   ast, std::ostream& os ) const { 
			DBG_BEGIN( String );
			os << '"' << ast->value << '"'; 
			DBG_END( String );
		}
		void visit( ConstVariablePtr ast, std::ostream& os ) const { 
			DBG_BEGIN( Variable );
			os << ast->name;  
			DBG_END( Variable );
		}
		void visit( ConstQuotePtr    ast, std::ostream& os ) const { 
			DBG_BEGIN( Quote );
			os << "'" << ast->value; 
			DBG_END( Quote );
		}
		void visit( ConstIfPtr       ast, std::ostream& os ) const { 	
			DBG_BEGIN( If );
 			os << "(if " << ast->test << " " << ast->thenBranch << " " << ast->elseBranch << ")"; 
			DBG_END( If );
		}
		void visit( ConstDoPtr       ast, std::ostream& os ) const {
			DBG_BEGIN( Do );
			os << "(do";
			for ( auto it = ast->exprs.begin(), end = ast->exprs.end(); it != end; ++it ) {
				os << ' ' << (*it);
			}
			os << ")";
			DBG_END( Do );
		}
		void visit( ConstDefinePtr   ast, std::ostream& os ) const {
			DBG_BEGIN( Define );
			os << "(define " << ast->name << " " << ast->expr << ")";
			DBG_END( Define );
		}
		void visit( ConstLetPtr      ast, std::ostream& os ) const {
			DBG_BEGIN( Let );
			os << "(let ";

			visit_( ast->bindings.front(), os );
			for ( auto it = ++(ast->bindings.begin()), end = ast->bindings.end(); it != end; ++it ) {
				os << ' ';
				visit_( *it, os );
			}

			os << " " << ast->body;
			os << ")";
			DBG_END( Let );
		}
		void visit( ConstLetStarPtr   ast, std::ostream& os ) const {
			DBG_BEGIN( LetStar );
			os << "(let* ";

			visit_( ast->bindings.front(), os );
			for ( auto it = ++(ast->bindings.begin()), end = ast->bindings.end(); it != end; ++it ) {
				os << ' ';
				visit_( *it, os );
			}

			os << " " << ast->body;
			os << ")";
			DBG_END( Let );
		}
		void visit( ConstLambdaPtr ast, std::ostream& os ) const {
			DBG_BEGIN( Lambda );
			os << "(lambda ";

			if ( std::strcmp( ast->name, "" ) != 0 ) {
				os << ast->name << ' ';
			}

			os << "(";
			
			if ( ast->arity() > 0 ) {
				os << ast->params.front();
				for ( auto it = ++(ast->params_begin()), end = ast->params_end(); it != end; ++it ) {
					os << " " << *it;
				}
			}	

			os << ") " << ast->body << ")";
			DBG_END( Lambda );
		}
		void visit( ConstApplicationPtr ast, std::ostream& os ) const {
			DBG_BEGIN( Application );
 			os << '(';
			os << ast->fun;
			for ( auto it = ast->args.begin(), end = ast->args.end(); it != end; ++it ) {
				os << ' ' << *it;
			}
			os << ')';
			DBG_END( Application );
		}

		void visit_( const std::pair<util::InternedString,AstPtr>& binding, std::ostream& os ) const {
			os << '(' << binding.first << ' ' << binding.second << ')';
		}
	};

	ast->visit<void,Visitor,std::ostream&>( Visitor(), os );

	return os;
}

