
#include "reader/Tokenizer.hpp"

#include "util/fail.hpp"

#include <fstream>
#include <cstring>
#include <cassert>

//#include <iostream>

using namespace lllm;

static inline CStr skipWhitespace( SourceLocation& sl, CStr src ) {
//	std::cout << "SKIPPIN WHITESPACE FROM '" << src << "'" << std::endl;

	while ( true ) {
		char c = *src;

		if ( c == '\n' ) {
			sl.incLine();
		} else if ( c == ' ' || c == '\t' ) {
			sl.incColumn();
		} else {
			break;
		}

		src++;
	}

//	std::cout << "SKIPPED WHITESPACE TO '" << src << "'" << std::endl;

	return src;
}
static inline CStr readStringLiteral( SourceLocation& sl, CStr src ) {
	assert( *src == '"' );

	while ( true ) {
		src++;

		switch ( *src ) {
			case 0:
				LLLM_FAIL( sl << ": EOF in string literal" );
			case '\n':
				LLLM_FAIL( sl << ": new line in string literal" );
			case '"':
				return src;
		}
	}
}


Tokenizer Tokenizer::fromString( CStr source ) {
	return fromString( "*string*", source );
}
Tokenizer Tokenizer::fromString( CStr sourceName, CStr source ) {
	return Tokenizer( sourceName, source );
}
Tokenizer Tokenizer::fromFile( CStr fileName ) {
	std::ifstream file( fileName );

	size_t begin = file.tellg();
  	file.seekg(0, std::ios::end);
	size_t end = file.tellg();
	
	size_t size = end - begin;

	char* src = new char[size + 1];

	file.read( src, size );

	assert( file.gcount() == size );

	src[size] = 0;

	return fromString( fileName, src );
}

bool Tokenizer::advance() {
//	std::cout << "ADVANCIN" << std::endl;

	loc.incColumn( end - start );
	end = start = skipWhitespace( loc, end );

	// check for special kinds of tokens
	switch ( *start ) {
		case 0:
			return false;
		case '(':   // start list
		case ')':   // end   list
		case '\'':  // quote
			end++;
			return true;
		case '"':  // start string literal
			end = readStringLiteral( loc, start );
			return true;
	}

	// read token
	do {
		switch ( *end ) {
			case 0:
			case ' ':
			case '\t':
				return true;
			case '\n':
				loc.incLine();
				return true;
			case '(':  // start list
			case ')':  // end   list
				return true;
		}
		end++;
	} while ( true );

	return false;
}

CStr Tokenizer::token()    {
//	std::cout << "GETTIN TOKEN OF LEN " << (end - start) << std::endl;

	if ( end == start ) return nullptr;

	char la = *start;
	if ( la == '(' ) return PAREN_OPEN;
	if ( la == ')' ) return PAREN_CLOSE;

	const size_t len   = end - start;
	char*        token = new char[len + 1];

	std::strncpy( token, start, len );
	token[len] = 0;

	return token;
}
const SourceLocation& Tokenizer::location() { return loc; }

Tokenizer::Tokenizer( CStr sourceName, CStr src ) : loc( sourceName ), src( src ), start( src ), end( src ) {}


