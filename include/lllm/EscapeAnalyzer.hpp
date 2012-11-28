#ifndef __LLLM_ESCAPE_ANALYZER_HPP__
#define __LLLM_ESCAPE_ANALYZER_HPP__ 1

#include "lllm/lllm.hpp"
//#include "lllm/ast/Ast.hpp"
//#include "lllm/sexpr/Sexpr.hpp"
//#include "lllm/GlobalScope.hpp"

namespace lllm {
	struct EscapeAnalyzer {
		static void analyze( ast::LambdaPtr ast, util::ScopePtr<ast::VariablePtr> env );
	};
};


#endif /* __LLLM_ESCAPE_ANALYZER_HPP__ */

