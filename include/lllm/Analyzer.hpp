#ifndef __ANALYZER_HPP__
#define __ANALYZER_HPP__ 1

#include "lllm/ast/Ast.hpp"
#include "lllm/sexpr/Sexpr.hpp"
#include "lllm/GlobalScope.hpp"

namespace lllm {
	class Analyzer {
		public:
			static ast::AstPtr analyze( sexpr::SexprPtr, GlobalScopePtr scope );
	};
};

#endif /* __ANALYZER_HPP__ */
