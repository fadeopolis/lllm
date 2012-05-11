#ifndef __ANALYZER_HPP__
#define __ANALYZER_HPP__ 1

#include "lllm/reader/Sexpr.hpp"
#include "lllm/analyzer/Ast.hpp"
#include "lllm/analyzer/Scope.hpp"

namespace lllm {
	// create Ast for a top level expr
	analyzer::AstPtr analyze( reader::SexprPtr, analyzer::GlobalScopePtr scope );		
};

#endif /* __ANALYZER_HPP__ */

