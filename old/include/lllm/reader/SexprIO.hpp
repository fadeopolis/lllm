#ifndef __SEXPR_IO_HPP__
#define __SEXPR_IO_HPP__

#include "lllm/reader/Sexpr.hpp"

#include <iosfwd>

namespace lllm {
	namespace reader {
		std::ostream& operator<<( std::ostream&, SexprPtr );
		std::ostream& operator<<( std::ostream&, SexprRef );
	}; // end namespace reader
}; // end namespace lllm

#endif /* __SEXPR_IO_HPP__ */

