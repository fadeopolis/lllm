#ifndef __ENV_HPP__
#define __ENV_HPP__ 1

#include "Value.hpp"

namespace lllm {
	typedef const class Value* ValuePtr;
	typedef const class Env*   EnvPtr;

	class Env {
		public:
			static EnvPtr make();

			virtual EnvPtr store( const char* key, ValuePtr val )   const = 0;
			virtual bool   contains( const char* key )              const = 0;
			virtual bool   lookup( const char* key, ValuePtr* dst ) const = 0;
	};
}

#endif /* __ENV_HPP__ */

