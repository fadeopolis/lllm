#ifndef __ENV_HPP__
#define __ENV_HPP__ 1

#include "Value.hpp"

namespace lllm {
	typedef const class Value* ValuePtr;
	typedef const class Env*   EnvPtr;

	class Env {
		public:
			static EnvPtr make();

			virtual EnvPtr merge( EnvPtr )                          const = 0;

			virtual EnvPtr store( const char* key, ValuePtr val )   const = 0;
			virtual bool   contains( const char* key )              const = 0;
			virtual bool   lookup( const char* key, ValuePtr* dst ) const = 0;

			EnvPtr store( SymbolPtr key, ValuePtr val )   const;
			bool   contains( SymbolPtr key )              const;
			bool   lookup( SymbolPtr key, ValuePtr* dst ) const;
		
			// cast to symbol and then invoke on string of symbol
			EnvPtr store( ValuePtr key, ValuePtr val )   const;
			bool   contains( ValuePtr key )              const;
			bool   lookup( ValuePtr key, ValuePtr* dst ) const;
	};
}

#endif /* __ENV_HPP__ */

