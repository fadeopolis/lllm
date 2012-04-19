#ifndef __VALUE_IO_HPP__
#define __VALUE_IO_HPP__ 1

#include "Value.hpp"

#include <iostream>

namespace lllm {
	enum class Type : long;

	std::ostream& operator<<( std::ostream&, Type );
	std::ostream& operator<<( std::ostream&, ValuePtr );
	std::ostream& operator<<( std::ostream&, const Value& );
}

#endif /* __VALUE_IO_HPP__ */

