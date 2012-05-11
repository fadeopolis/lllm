#ifndef __ANALYZER_HPP__
#define __ANALYZER_HPP__ 1

#include "lllm/ast/Ast.hpp"
#include "lllm/sexpr/Sexpr.hpp"
#include "lllm/util/Scope.tpp"

namespace lllm {
	typedef util::Scope<ast::VariablePtr>    AnalyzerScope;
	typedef util::ScopePtr<ast::VariablePtr> AnalyzerScopePtr;

	class Analyzer {
		public:
			static ast::AstPtr analyze( sexpr::SexprPtr, AnalyzerScopePtr scope );
	};
};

#endif /* __ANALYZER_HPP__ */

