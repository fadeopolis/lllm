#ifndef __READER_HPP__
#define __READER_HPP__ 1

#include "Value.hpp"

#include <iosfwd>
#include <string>

namespace lllm {
	// parse from string
	ValuePtr read( const char* str );
	// parse from file
	ValuePtr readFile( const char* name );
}

#endif /* __READER_HPP__ */


