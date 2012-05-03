#ifndef __GLOBALS_HPP__
#define __GLOBALS_HPP__

#include "lllm/predef.hpp"
#include "lllm/analyzer/Scope.hpp"

namespace lllm {
	class VM {
		public:
			static VM* make();

			eval::EnvPtr             env();
			analyzer::GlobalScopePtr scope();
		private:
			VM();
			VM( const VM& ) = delete;
			
			analyzer::GlobalScope _scope;
			eval::EnvPtr          _env;
	};
};

#endif /* __GLOBALS_HPP__ */

