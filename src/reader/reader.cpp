
#include "lllm/reader.hpp"
#include "lllm/util/fail.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstring>
#include <algorithm>

using namespace std;
using namespace lllm;
using namespace lllm::util;
using namespace lllm::reader;

reader::SexprPtr lllm::read( CStr str ) {
	return Reader::fromString( str ).read();
}
reader::SexprPtr lllm::read( CStr srcName, CStr str ) {
	return Reader::fromString( srcName, str ).read();
}

Reader Reader::fromString( CStr source ) {
	return fromString( "*string*", source );
}
Reader Reader::fromString( CStr sourceName, CStr source ) {
	return Reader( sourceName, new stringstream( source ) );
}
	
Reader Reader::fromFile( CStr fileName ) {
	return Reader( fileName, new fstream( fileName ) );
}
Reader Reader::fromStdin() {
	return Reader( "*stdin*", &cin );
}

Reader::Reader( CStr srcName, std::istream* src ) : stream( src ), loc( srcName ) {
	la = stream->get();
}

Reader::~Reader() { 
	if ( stream != &cin ) delete stream;
}

SexprPtr Reader::read() {
	skipWhitespace();

	// return null if we reach EOF
	if ( la < 0 ) return nullptr;

	// at this point there is at least one 
	// valid char left in the input

//	cout << "DISPATCHING " << (char)la << endl;

	switch ( la ) {
		case '(':  return readList();
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
		case '9':  return readNumber();
		case '\\': return readChar();
		case '\'': return readQuote();
		case '"':  return readString();
		default:   return readSymbol();
	}

	return nullptr;
}

ListPtr   Reader::readList() {
	SourceLocation start = loc;

	consume( '(' );
	loc.incColumn();	

	std::vector<SexprPtr> exprs;

	while ( true ) {
		skipWhitespace();

		if ( la < 0 ) LLLM_FAIL( loc << ": Unexpected EOF while reading a list" );

		if ( la == ')' ) {
			consume(')');
			loc.incColumn();
			break;
		}

		exprs.push_back( read() );
	}

	return new List( start, exprs );
}

ListPtr   Reader::readQuote() {
	SourceLocation start = loc;

	consume( '\'' );

	SymbolPtr sym = new Symbol( start, "quote" );
	loc.incColumn();

	SexprPtr val = read();

	if ( !val ) LLLM_FAIL( loc << ": Unexpected EOF while reading a quotation" );

	std::vector<SexprPtr> exprs;
	exprs.push_back( sym );
	exprs.push_back( val );

	return new List( start, exprs );
}

SexprPtr  Reader::readNumber() {
	SourceLocation start = loc;

	assert( ('0' <= la && la <= '9') || (la == '_') || (la == '.') );

	std::stringstream str;

	while ( true ) {
		if ( la < 0 ) goto return_int;

		switch ( la ) {
			case ' ':
			case '\t':
			case '\n':
			case '(':
			case ')':
				goto return_int;
			case '.':
				str << '.';
				loc.incColumn();
				goto read_real;
			case '_':
				loc.incColumn();
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
				str << ((char)la);
				loc.incColumn();
				break;
			default:
				LLLM_FAIL( loc << ": Illegal character '" << la << "' in number literal" );
				break;
		}

		la = stream->get();
	}
read_real:
	while ( true ) {
		la = stream->get();

		if ( la < 0 ) goto return_real;

		switch ( la ) {
			case ' ':
			case '\t':
			case '\n':
			case '(':
			case ')':
				goto return_real;
			case '.':
				LLLM_FAIL( loc << ": More than one decimal point in a number literal" );
				break;
			case '_':
				loc.incColumn();
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
				str << ((char)la);
				loc.incColumn();
				break;
			default:
				LLLM_FAIL( loc << ": Illegal character '" << la << "' in number literal" );
				break;
		}
	}

return_int:
		long l;
		str >> l;
		return new Int( start, l );
return_real:
		double d;
		str >> d;
		return new Real( start, d );
}

CharPtr   Reader::readChar() {
	SourceLocation start = loc;

	consume( '\\' );
	loc.incColumn();

	stringstream buf;

	while ( true ) {
		la = stream->get();

		if ( la < 0 ) goto loop_end;

		switch ( la ) {
			case ' ':
			case '\t':
			case '\n':
			case '(':
			case ')':
				goto loop_end;
			default:
				buf << ((char)la);
				loc.incColumn();
		}
	}
loop_end:
	const std::string& str = buf.str();

	if ( str.size() == 1 ) {
		return new Char( start, str[0] );
	} else if ( str == "tab" ) {
		return new Char( start, '\t' );
	} else if ( str == "newline" ) {
		return new Char( start, '\n' );
	} else {
		LLLM_FAIL( loc << ": Illegal character literal '\\" << str << "'" );
	}
}

StringPtr Reader::readString() {
	SourceLocation start = loc;

	consume( '"' );
	loc.incColumn();

	stringstream buf;

	while ( true ) {
		if ( la < 0 ) LLLM_FAIL( loc << ": Unexpected EOF while reading a string" );

		if ( la == '"' ) {
			consume( '"' );
			loc.incColumn();
			break;
		}

		buf << ((char) la);

		la = stream->get();
		loc.incColumn();
	}

	const std::string& str = buf.str();
	char*              out = new char[str.size()];

	strcpy( out, str.c_str() );

	return new String( start, out );
}

SymbolPtr Reader::readSymbol() {
	SourceLocation start = loc;

	stringstream buf;

	while ( true ) {
		buf << ((char) la);

		la = stream->get();

		if ( la < 0 ) goto loop_end;

		switch ( la ) {
			case ' ':
			case '\t':
			case '\n':
			case '(':
			case ')':
				goto loop_end;
			case '\'':
				LLLM_FAIL( loc << ": Unexpected ' in symbol" );
			case '\\':
				LLLM_FAIL( loc << ": Unexpected \\ in symbol" );
			default:
				loc.incColumn();
				break;
		}
	}
loop_end:
	const std::string& str = buf.str();
	char*              out = new char[str.size()];

	strcpy( out, str.c_str() );

	return new Symbol( start, out );
}

void Reader::consume( char expected ) {
	assert( la == expected );

	la = stream->get();
}

void Reader::skipWhitespace() {
	while ( true ) {
		switch ( la ) {
			case '\n':
				loc.incLine();
				break;
			case ' ':
			case '\t':
				loc.incColumn();
				break;
			default:
				return;
		}

		la = stream->get();
	}
}

