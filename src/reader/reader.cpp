
#include "reader.hpp"

#include "reader/Tokenizer.hpp"
#include "util/fail.hpp"

#include <sstream>
#include <cassert>
#include <cstring>

using namespace lllm;

static ParseTree* _read( Tokenizer& ts );

static ListTree*   _readList( Tokenizer& ts );
static ListTree*   _readQuote( const SourceLocation& loc, Tokenizer& ts );
static NumberTree* _readNumber( const SourceLocation& loc, CStr tok );

static CharTree*   _readChar( const SourceLocation& loc, CStr tok );
static StringTree* _readString( const SourceLocation& loc, CStr tok );
static SymbolTree* _readSymbol( const SourceLocation& loc, CStr tok );

ParseTree* lllm::read( CStr string ) {
	Tokenizer t = Tokenizer::fromString( string );
	return _read( t );
}
ParseTree* lllm::readFile( CStr fileName ) {
	Tokenizer t = Tokenizer::fromFile( fileName );
	return _read( t );
}

ParseTree* _read( Tokenizer& ts ) {
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

ListTree*   _readList( Tokenizer& ts ) {
	SourceLocation start = ts.location();

	ParseTree* car;
	ListTree*  cdr;

	while ( ts.advance() ) {
		const SourceLocation& loc = ts.location();
		CStr                  tok = ts.token();

		assert( tok );

		switch ( *tok ) {
			case '(':  
				car = _readList( ts );
				cdr = _readList( ts );
				return new ListTree( start, car, cdr );
			case ')':  return nullptr;
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
				car = _readNumber( loc, tok );
				cdr = _readList( ts );
				return new ListTree( start, car, cdr );
			case '\\':
				car = _readChar( loc, tok );
				cdr = _readList( ts );
				return new ListTree( start, car, cdr );
			case '\'':
				car = _readQuote( loc, ts );
				cdr = _readList( ts );
				return new ListTree( start, car, cdr );
			case '"':
				car = _readString( loc, tok );
				cdr = _readList( ts );
				return new ListTree( start, car, cdr );
			default:
				car = _readSymbol( loc, tok );
				cdr = _readList( ts );
				return new ListTree( start, car, cdr );
		}
	}

	LLLM_FAIL( ts.location() << ": EOF while reading list" );
}
ListTree*   _readQuote( const SourceLocation& loc, Tokenizer& ts ) {
	ParseTree* value = _read( ts );

	return new ListTree( loc, new SymbolTree( loc, "quote" ), new ListTree( loc, value, nullptr ) );
}
NumberTree* _readNumber( const SourceLocation& loc, CStr tok ) {
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
		return new IntTree( loc, l );
return_real:
		double d;
		str >> d;
		return new RealTree( loc, d );
}

CharTree*   _readChar( const SourceLocation& loc, CStr tok ) {
	assert( *tok == '\\' );

	tok++;

	if ( std::strcmp( "tab", tok ) == 0 ) {
		return new CharTree( loc, '\t' );	
	}
	if ( std::strcmp( "newline", tok ) == 0 ) {
		return new CharTree( loc, '\n' );	
	}

	assert( tok[1] == 0 );

	return new CharTree( loc, *tok );	
}
StringTree* _readString( const SourceLocation& loc, CStr tok ) {
	return new StringTree( loc, tok );
}
SymbolTree* _readSymbol( const SourceLocation& loc, CStr tok ) {
	return new SymbolTree( loc, tok );
}

