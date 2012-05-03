#ifndef __TOKENIZER_HPP__
#define __TOKENIZER_HPP__ 1

#include "lllm/reader/SourceLocation.hpp"

namespace lllm {
	typedef const char* CStr;

	namespace reader {
		class Tokenizer {
			public:
				static const CStr PAREN_OPEN;
				static const CStr PAREN_CLOSE;
	
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
	}; // end namespace reader
}; // end namespace lllm


#endif /* __TOKENIZER_HPP__ */

