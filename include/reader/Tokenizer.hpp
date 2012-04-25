#ifndef __TOKENIZER_HPP__
#define __TOKENIZER_HPP__ 1

#include "reader/SourceLocation.hpp"

namespace lllm {
	typedef const char* CStr;

	class Tokenizer {
		public:
			static constexpr CStr PAREN_OPEN  = "(";
			static constexpr CStr PAREN_CLOSE = ")";

			static Tokenizer fromString( CStr source );
			static Tokenizer fromString( CStr sourceName, CStr source );

			static Tokenizer fromFile( CStr fileName );

			bool advance();
		
			CStr                  token();
			const SourceLocation& location();
		private:
			Tokenizer( CStr sourceName, CStr src );
			
			const CStr src;
			CStr start, end;
			SourceLocation loc;
	};
};


#endif /* __TOKENIZER_HPP__ */

