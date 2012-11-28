
#include "lllm/EscapeAnalyzer.hpp"

#include "lllm/ast/Ast.hpp"
#include "lllm/value/Value.hpp"

using namespace lllm;
using namespace lllm::ast;
using namespace lllm::util;

void EscapeAnalyzer::analyze( ast::LambdaPtr ast, util::ScopePtr<ast::VariablePtr> env ) {
	if ( !ast ) return;

	// on application:    val.escape = max( val.escape, param.escape  )
	// on return:         val.escape = max( val.escape, ESCAPE_RETURN )
	// on 2nd arg to set: val.escape = max( val.escape, ref.escape    )

	struct Visitor {
		// literals are easy
		void visit( ast::NilPtr      ast, bool tail ) {}
		void visit( ast::IntPtr      ast, bool tail ) {
			if ( tail ) ast->escape = max( ast->escape, EscapeStatus::ESCAPE_AS_RETURN );
		}
		void visit( ast::RealPtr     ast, bool tail ) {
			if ( tail ) ast->escape = max( ast->escape, EscapeStatus::ESCAPE_AS_RETURN );
		}
		void visit( ast::CharPtr     ast, bool tail ) {
			if ( tail ) ast->escape = max( ast->escape, EscapeStatus::ESCAPE_AS_RETURN );
		}
		void visit( ast::StringPtr   ast, bool tail ) {
			if ( tail ) ast->escape = max( ast->escape, EscapeStatus::ESCAPE_AS_RETURN );
		}
		// variables 
		void visit( ast::VariablePtr ast, bool tail ) {
			
		}
		void visit( ast::LambdaPtr   ast, bool tail ) {
			
		}
		void visit( ast::AstPtr    ast, bool tail ) {
			
		}

		util::ScopePtr<ast::VariablePtr> env;
	};	

	Visitor v{ env };
	ast->body->visit<void>( v, true );
}

