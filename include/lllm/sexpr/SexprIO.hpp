#ifndef __SEXPR_IO_HPP__
#define __SEXPR_IO_HPP__ 1

#include "lllm/sexpr/Sexpr.hpp"

#include <iosfwd>

namespace lllm {
	std::ostream& operator<<( std::ostream&, sexpr::SexprPtr );	
};

#endif /* __SEXPR_IO_HPP__ */

