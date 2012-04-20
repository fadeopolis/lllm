
#include "read.hpp"
#include "Source.hpp"
#include "values.hpp"

#include <cassert>
#include <cstring>
#include <sstream>

using namespace std;
using namespace lllm;
using namespace lllm::val;

#define LOG( SRC, MSG ) cout << __FUNCTION__ << "\t" << SRC << " " << MSG << endl
#//#define LOG( SRC, MSG )

static ValuePtr _read( Source& src );
static ValuePtr _readList( Source& src );
static ValuePtr _readTail( Source& src );
static ValuePtr _readQuote( Source& src );
static ValuePtr _readAtom( Source& src );
static ValuePtr _readChar( Source& src );
static ValuePtr _readNumber( Source& src );
static ValuePtr _readString( Source& src );
static ValuePtr _readSymbol( Source& src );

static void skipWhitespace( Source& src );
static void skipComment( Source& src );
static void fail( Source& src, const std::string& msg );

ValuePtr lllm::read( const char* str ) {
	StringSource src( str );
	return _read( src );
}
ValuePtr lllm::readFile( const char* name ) {
	FileSource src( name );
	return _read( src );
}

#define FAIL( SRC, MSG ) ({   \
	std::stringstream str;    \
	str << MSG;               \
	fail( (SRC), str.str() ); \
})

ValuePtr _read( Source& src ) {
	LOG( src, "" );
	skipWhitespace( src );

	if ( !src ) fail( src, "EOF while reading expression" );
	
	switch ( src.peek() ) {
		case '(':  return _readList( src );
		case '\'': return _readQuote( src );
		default:   return _readAtom( src );
	}
}
ValuePtr _readList( Source& src ) {
	LOG( src, "" );
	assert( src && src.consume('(') );

	skipWhitespace( src );
	if ( !src ) fail( src, "EOF while reading list" );
	
	// if we immediately read ) this is an empty list
	if ( src.peek() == ')' ) {
		return nil();
		src.consume();
	}

	ValuePtr car = _read( src );
	ValuePtr cdr = _readTail( src );

	LOG( src, "READ: " << cons( car, cdr ) );

	return cons( car, cdr );
}
ValuePtr _readTail( Source& src ) {
	LOG( src, "" );
	assert( src );

	skipWhitespace( src );
	if ( !src ) fail( src, "EOF while reading list" );

	if ( src.peek() == ')' ) {
		src.consume();
		return nil();
	}
	
	ValuePtr car = _read( src );
	ValuePtr cdr = _readTail( src );

	LOG( src, "READ: " << cons( car, cdr ) );	

	return cons( car, cdr );
}
ValuePtr _readQuote( Source& src ) {
	LOG( src, "" );
	assert( src && src.consume('\'') );

	ValuePtr quoted = _read( src );

	LOG( src, "READ: '" << quoted );

	return cons( symbol("quote"), cons( quoted, nil() ) );
}
ValuePtr _readAtom( Source& src ) {
	LOG( src, "" );
	assert( src );

	switch ( src.peek() ) {
		case '\\': return _readChar( src );
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':  return _readNumber( src );
		case '"':  return _readString( src );
		default:   return _readSymbol( src );
	}
}
ValuePtr _readChar( Source& src ) {
	LOG( src, "" );
	assert( src && src.consume('\\') );

	if ( !src ) fail( src, "EOF while reading character" );

	LOG( src, "READ: " << src.peek() );

	return val::character( src.read() );
}
ValuePtr _readNumber( Source& src ) {
	LOG( src, "" );
	assert( src );

	std::stringstream buf;

	while ( true ) {
		char c = src.peek();

		if ( !src ) goto end;

		switch ( c ) {
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
				buf << c;
				break;
			// skip underscores in numbers
			case '_':
				break;
			// any other character terminates a the number
			default:
				goto end;	
		}
		src.read();
	}

	end:
		int64_t i;
		buf >> i;
		LOG( src, "READ: " << i );
		return number( i );
}
ValuePtr _readString( Source& src ) {
	LOG( src, "" );
	assert( src && src.peek() == '"' );

	std::stringstream buf;

	// skip "
	src.read();
	
	while ( true ) {
		char c = src.peek();

		if ( !src ) fail( src, "EOF while reading string" );

		if ( src.consume('"') ) {
			char* cs = (char*) malloc( buf.str().size() );
			std::strcpy( cs, buf.str().c_str() );
			LOG( src, "READ: \"" << cs << '"' );
			return val::string( cs );
		}

		buf << c;

		src.read();
	}
}
ValuePtr _readSymbol( Source& src ) {
	LOG( src, "" );
	assert( src );

	std::stringstream buf;

	while ( true ) {
		char c = src.peek();

		if ( !src ) goto end;

		switch ( c ) {
			case ';':
			case ' ':
			case '\t':
			case '\n':
			case '\\':
			case '"':
			case '\'':
			case '(':
			case ')':
				goto end;
			default:
				buf << c; LOG( src, "PUTTING '" << c << "', LA='" << src.peek() << "'" );
		}
		c = src.read();
	}

	end:
		char* cs = (char*) malloc( buf.str().size() );
		std::strcpy( cs, buf.str().c_str() );
		LOG( src, "READ: " << cs );
		return symbol( cs );
}

void skipWhitespace( Source& src ) {
	LOG( src, "" );
	while ( true ) {
		if ( !src ) return;

		switch ( src.peek() ) {
			case ';':
				skipComment( src );
				continue;
			case ' ':  
			case '\t': 
			case '\n':
				src.read();
				continue;	
			default:
				return;
		}
	}
}
void skipComment( Source& src ) {
	LOG( src, "" );
	assert( src && src.consume(';') );

	src.consume(); // skip ;

	while ( true ) {
		if ( !src ) return;

		switch ( src.read() ) {
			case '\n':
				return;	
			default:
				continue;
		}
	}
}
void fail( Source& src, const std::string& msg ) {
	std::cerr << src << ":" << msg << std::endl;
	exit( 1 );
}

