#ifndef __LLLM_ENV_TPP__
#define __LLLM_ENV_TPP__ 1

#include "lllm/lllm.hpp"
#include "lllm/util/InternedString.hpp"

namespace lllm {
	namespace util {
		template<typename T> 
		class Scope {
			public:
				virtual bool contains( const util::InternedString& name ) = 0;
				virtual bool lookup( const util::InternedString& name, T* dst ) = 0;

				virtual void dump() {}
		};
	};
};

#endif /* __LLLM_ENV_TPP__ */

