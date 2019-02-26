#ifndef __VALUE_IO_HPP__
#define __VALUE_IO_HPP__

#include "lllm/value/Value.hpp"

#include <iosfwd>

namespace lllm {
	std::ostream& operator<<( std::ostream&, value::Type );
	std::ostream& operator<<( std::ostream&, value::ValuePtr );
};

#endif /* __VALUE_IO_HPP__ */
