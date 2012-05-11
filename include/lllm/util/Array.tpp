#ifndef __LLLM_ARRAY_TPP__
#define __LLLM_ARRAY_TPP__ 1

#include "lllm/lllm.hpp"

namespace lllm {
	namespace util {
		template<typename T>
		class Array {
			public:
				typedef T* iterator;

				inline constexpr Array( size_t len, T* begin );
				inline constexpr Array( T* begin, T* end );

				template<typename... Ts>
				inline Array( Ts... ts );
			private
				template<typename... Ts> static inline constexpr len();
		};

		constexpr Array::Array( size_t len, T* begin ) : _begin( begin ), _end( begin + len ) {}
		constexpr Array::Array( T* begin, T* end )     : _begin( begin ), _end( end ) {}

		template<typename... Ts>
		Array::Array( Ts... ts ) :
			private
				template<typename... Ts> static inline constexpr len();

	};	
};

#endif /* __LLLM_ARRAY_TPP__ */

