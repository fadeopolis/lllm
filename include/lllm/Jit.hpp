#ifndef __LLLM_JIT_HPP__
#define __LLLM_JIT_HPP__ 1

#include "lllm/value/Value.hpp"
#include "lllm/util/Scope.tpp"

namespace lllm {
	class Jit {
		public:
			static void setInliningThreshold( size_t depthThreshold );
			static void setInliningDepth( size_t maxRecurison );

			static void compile( value::LambdaPtr fn, util::ScopePtr<value::ValuePtr> globals );

			static ast::LambdaPtr performInlining( ast::LambdaPtr fn, util::ScopePtr<value::ValuePtr> globals );
		private:
			Jit( const Jit& ) = delete;
			Jit& operator=( const Jit& ) = delete;

			struct SharedData;
			static SharedData* shared; 

			static size_t inliningThreshold;
			static size_t inliningDepth;
	};
};

#endif /* __LLLM_JIT_HPP__ */

