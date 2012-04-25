#ifndef __READER_HPP__
#define __READER_HPP__ 1

#include "reader/ParseTree.hpp"

namespace lllm {
	typedef const char* CStr;

	ParseTree* read( CStr string );
	ParseTree* readFile( CStr fileName );
};

#endif /* __READER_HPP__ */

