#ifndef __READER_HPP__
#define __READER_HPP__ 1

#include "lllm/lllm.hpp"
#include "lllm/sexpr/Sexpr.hpp"

#include <iosfwd>

namespace lllm {
	class Reader {
		public:
			static sexpr::SexprPtr read( util::CStr str );
			static sexpr::SexprPtr read( util::CStr srcName, util::CStr str );

			static Reader fromString( util::CStr source );
			static Reader fromString( util::CStr sourceName, util::CStr source );

			static Reader fromFile( util::CStr fileName );
			static Reader fromStdin();

			~Reader();

			sexpr::SexprPtr read();
		private:
			Reader( util::CStr srcName, std::istream* src );

			sexpr::ListPtr   readList();
			sexpr::ListPtr   readQuote();
			sexpr::SexprPtr  readNumber();
			sexpr::CharPtr   readChar();
			sexpr::StringPtr readString();
			sexpr::SymbolPtr readSymbol();

			void skipComment();
			void skipWhitespace();
			void consume( char expected );

			int                  la;
			std::istream*        stream;
			util::SourceLocation loc;
	};
};

#endif /* __READER_HPP__ */
