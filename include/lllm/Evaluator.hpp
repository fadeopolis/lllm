#ifndef __EVALUATOR_HPP__
#define __EVALUATOR_HPP__ 1

#include "lllm/ast/Ast.hpp"
#include "lllm/value/Value.hpp"
#include "lllm/util/InternedString.hpp"
#include "lllm/util/Scope.tpp"

namespace lllm {
	class Evaluator {
		public:
			static void setJittingThreshold( size_t threshold );

			static value::ValuePtr evaluate( ast::AstPtr ast, const util::ScopePtr<value::ValuePtr> env );
		private:
			static value::ValuePtr applyFun( value::LambdaPtr fn, size_t arity, value::Lambda::FnPtr code, const std::vector<value::ValuePtr>& args );
			static value::ValuePtr applyAST( value::LambdaPtr fn, util::ScopePtr<value::ValuePtr> env, const std::vector<value::ValuePtr>& args );


			static size_t jittingThreshold;
	};
};

#endif /* __EVALUATOR_HPP__ */

