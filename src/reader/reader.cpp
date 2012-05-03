
#include "lllm/reader.hpp"
#include "lllm/reader/Tokenizer.hpp"
#include "lllm/util/fail.hpp"

#include <sstream>
#include <cassert>
#include <cstring>
#include <algorithm>

using namespace lllm;
using namespace lllm::util;
using namespace lllm::reader;

static SexprPtr _read( Tokenizer& ts );

static ListPtr   _readList( Tokenizer& ts );
static ListPtr   _readQuote( const SourceLocation& loc, Tokenizer& ts );
static SexprPtr  _readNumber( const SourceLocation& loc, CStr tok );

static CharPtr   _readChar( const SourceLocation& loc, CStr tok );
static StringPtr _readString( const SourceLocation& loc, CStr tok );
static SymbolPtr _readSymbol( const SourceLocation& loc, CStr tok );

SexprPtr lllm::read( CStr string ) {
	Tokenizer t = Tokenizer::fromString( string );
	return _read( t );
}
SexprPtr lllm::readFile( CStr fileName ) {
	Tokenizer t = Tokenizer::fromFile( fileName );
	return _read( t );
}

SexprPtr _read( Tokenizer& ts ) {
	while ( ts.advance() ) {
		const SourceLocation& loc = ts.location();
		CStr                  tok = ts.token();

		assert( tok );

		switch ( *tok ) {
			case '(':  return _readList( ts );
			case ')':  LLLM_FAIL( "Unexpected ')' in " << loc );
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':  return _readNumber( loc, tok );
			case '\\': return _readChar( loc, tok );
			case '\'': return _readQuote( loc, ts );
			case '"':  return _readString( loc, tok );
			default:   return _readSymbol( loc, tok );
		}

		if ( tok == Tokenizer::PAREN_OPEN  ) return _readList( ts );
		if ( tok == Tokenizer::PAREN_CLOSE ) {
			LLLM_FAIL( "Unexpected ')' in " << loc );
		}
	}

	LLLM_FAIL( ts.location() << ": EOF while reading" );
}

ListPtr   _readList( Tokenizer& ts ) {
	SourceLocation start = ts.location();

	std::vector<SexprPtr> exprs;

	while ( ts.advance() ) {
		const SourceLocation& loc = ts.location();
		CStr                  tok = ts.token();

		assert( tok );

		switch ( *tok ) {
			case '(': 
				exprs.push_back( _readList( ts ) );
				break;
			case ')':  
				return new List( loc, exprs );
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				exprs.push_back( _readNumber( loc, tok ) );
				break;
			case '\\':
				exprs.push_back( _readChar( loc, tok ) );
				break;
			case '\'':
				exprs.push_back( _readQuote( loc, ts ) );
				break;
			case '"':
				exprs.push_back( _readString( loc, tok ) );
				break;
			default:
				exprs.push_back( _readSymbol( loc, tok ) );
				break;
		}
	}

	LLLM_FAIL( ts.location() << ": EOF while reading list" );
}
ListPtr   _readQuote( const SourceLocation& loc, Tokenizer& ts ) {
	std::vector<SexprPtr> exprs;

	exprs.push_back( new Symbol( loc, "quote" ) );
	exprs.push_back( _read( ts ) );

	return new List( loc, exprs );
}
SexprPtr _readNumber( const SourceLocation& loc, CStr tok ) {
//	std::cout << "READ NUM: " << tok << std::endl;
	std::stringstream str;

	str << *tok;

	while ( true ) {
		tok++;
		switch ( *tok ) {
			case 0:
				goto return_int;
			case '.':
				str << '.';
				goto read_real;
			case '_':
				break; // underscores in numbers are ignored
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				str << *tok;
				break;
			default:
				LLLM_FAIL( loc << ": Illegal character '" << (*tok) << "' in number literal" );
		}
	}
read_real:
	while ( true ) {
		tok++;
		switch ( *tok ) {
			case 0:
				goto return_real;
			case '.':
				LLLM_FAIL( loc << ": More than one decimal point in a number literal" );
			case '_':
				break; // underscores in numbers are ignored
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				str << *tok;
				break;
			default:
				LLLM_FAIL( loc << ": Illegal character '" << (*tok) << "' in number literal" );
		}
	}

return_int:
		long l;
		str >> l;
		return new Int( loc, l );
return_real:
		double d;
		str >> d;
		return new Real( loc, d );
}

CharPtr   _readChar( const SourceLocation& loc, CStr tok ) {
//	std::cout << "READ CHAR: " << tok << std::endl;
	assert( *tok == '\\' );

	tok++;

	if ( std::strcmp( "tab", tok ) == 0 ) {
		return new Char( loc, '\t' );	
	}
	if ( std::strcmp( "newline", tok ) == 0 ) {
		return new Char( loc, '\n' );	
	}

	assert( tok[1] == 0 );

	return new Char( loc, *tok );	
}

StringPtr _readString( const SourceLocation& loc, CStr tok ) {
//	std::cout << "READ STR: " << tok << std::endl;
	// drop leading ", the " at the end was dropped by tokenizer
	tok++;

	return new String( loc, tok );
}
SymbolPtr _readSymbol( const SourceLocation& loc, CStr tok ) {
//	std::cout << "READ SYM: " << tok << std::endl;
	return new Symbol( loc, tok );
}

