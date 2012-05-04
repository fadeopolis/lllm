#ifndef __READER_HPP__
#define __READER_HPP__ 1

#include "lllm/reader/Sexpr.hpp"

#include <iosfwd>

namespace lllm {
	class Reader {
		public:
			static Reader fromString( CStr source );
			static Reader fromString( CStr sourceName, CStr source );
	
			static Reader fromFile( CStr fileName );
			static Reader fromStdin();			

			~Reader();

			reader::SexprPtr read();
		private:
			Reader( CStr srcName, std::istream* src );

			reader::ListPtr   readList();
			reader::ListPtr   readQuote();
			reader::SexprPtr  readNumber();
			reader::CharPtr   readChar();
			reader::StringPtr readString();
			reader::SymbolPtr readSymbol();

			void skipWhitespace();
			void consume( char expected );

			int                    la;
			std::istream*          stream;
			reader::SourceLocation loc;
	};

	reader::SexprPtr read( CStr str );
	reader::SexprPtr read( CStr srcName, CStr str );
};

#endif /* __READER_HPP__ */

