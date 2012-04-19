
#include "Source.hpp"

#include <iostream>
#include <fstream>

using namespace lllm;

Source::Source() {}

StringSource::StringSource( const char* src ) : StringSource( "string", src ) {}
StringSource::StringSource( const char* name, const char* src ) : name( name ), str( src ), curLine( 0 ), curColumn( -1 ) {}

FileSource::FileSource( const char* name ) : name( name ), file( name ), la( 0 ), curLine( 0 ), curColumn( -1 ) {
	consume();
}

std::ostream& lllm::operator<<( std::ostream& os, const Source& s ) {
	s.print( os );
	return os;
}

int Source::read() {
	int i = peek();
	if ( i >= 0 ) consume();
	return i;
}

int  StringSource::peek() const { return (*str) ? ((int)*str) : -1 ; }
void StringSource::consume() {
	if ( *str ) {
		str++;
		if ( (*str) == '\n' ) {
			curLine++;
			curColumn = 0;
		} else {
			curColumn++;
		}
	}
}
bool StringSource::consume( char c ) {
	if ( *str ) {
		consume();
		return true;
	} else {
		return false;
	}
}

uint StringSource::line()   const { return curLine;   }
uint StringSource::column() const { return curColumn; }

void StringSource::print( std::ostream& os ) const {
	os << name << ':' << curLine << ':' << curColumn << ":LA='" << (peek() > 0 ? ((char)peek()) : ((char)0)) << "'";
}

StringSource::operator void*() const {
	return (*str) ? (void*)this : nullptr;
}	
bool StringSource::operator!() const {
	return !(*str);
}

int  FileSource::peek() const { return la; }
void FileSource::consume() {
	int i = file.get();

	std::swap( i, la );

	if ( la < 0 ) return;
	
	char c = la;
	
	if ( c == '\n' ) {
		curLine++;
		curColumn = 0;
	} else {
		curColumn++;
	}
}
bool FileSource::consume( char c ) {
	if ( la == c ) {
		consume();
		return true;
	} else {
		return false;
	}
}

uint FileSource::line()   const { return curLine;   }
uint FileSource::column() const { return curColumn; }

void FileSource::print( std::ostream& os ) const {
	os << name << ':' << curLine << ':' << curColumn;
}

FileSource::operator void*() const {
	return la >= 0 ? (void*)this : nullptr;
}	
bool FileSource::operator!() const {
	return la < 0;
}


