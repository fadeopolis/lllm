#ifndef __READER_HPP__
#define __READER_HPP__ 1

#include "lllm/reader/Sexpr.hpp"

namespace lllm {
	reader::SexprPtr read( CStr string );
	reader::SexprPtr readFile( CStr fileName );
};

#endif /* __READER_HPP__ */

