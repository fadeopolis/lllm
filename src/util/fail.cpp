
#include "lllm/util/fail.hpp"

#include <cstdlib>
#include <iostream>
#include <execinfo.h>
#include <cxxabi.h>

using namespace lllm;

#define MAX_FRAMES 100

static inline void demangle( const char* symbol, std::string& dst );

void util::fail( const char* file, const char* function, int line, const char* msg ) {
	void* stackFrames[MAX_FRAMES];

	int    size    = backtrace( stackFrames, MAX_FRAMES );

	// the top most frame is always the fail function, skip that
	size--;

	char** symbols = backtrace_symbols( stackFrames + 1, size );

	std::cout << "\nError: \""  << msg << '"' << std::endl;
	std::cout << "In file     " << file     << std::endl;
	std::cout << "In function " << function << std::endl;
	std::cout << "At line     " << line     << std::endl;
	std::cout << "Stack trace:" << std::endl;

	std::string buf;
	for ( int i = 0; i < size; i++ ) {
		demangle( symbols[i], buf );
		std::cout << '\t' << buf << std::endl;
	}

	free( symbols );

	std::abort();
}

void demangle( const char* symbol, std::string& dst ) {
	static char temp[128];

	dst.clear();

	size_t size;
	int status;
	char* demangled;

	//first, try to demangle a c++ name
	if ( 1 == sscanf( symbol, "%*[^(]%*[^_]%127[^)+]", temp ) ) {
	    if ( NULL != (demangled = abi::__cxa_demangle( temp, NULL, &size, &status )) ) {
			dst += demangled;
     		free(demangled);
			return;
		}
	}
	//if that didn't work, try to get a regular c symbol
	if (1 == sscanf( symbol, "%127s", temp )) {
		dst += temp;
	    return;
	}

	//if all else fails, just return the symbol
	dst += symbol;
	return;
}
