#ifndef __LLLM_JIT_HPP__
#define __LLLM_JIT_HPP__ 1

#include "lllm/value/Value.hpp"
#include "lllm/util/Scope.tpp"

namespace lllm {
	class Jit {
		public:
			static void compile( value::LambdaPtr fn, util::ScopePtr<value::ValuePtr> globals );
		private:
			Jit( const Jit& ) = delete;
			Jit& operator=( const Jit& ) = delete;

			struct SharedData;
			static SharedData* shared; 
	};
};

#endif /* __LLLM_JIT_HPP__ */

