#ifndef __VALUE_IO_HPP__
#define __VALUE_IO_HPP__ 1

#include "Value.hpp"

#include <iostream>

namespace lllm {
	class Value;
	enum class Type : long;

	std::ostream& operator<<( std::ostream&, Type );
	std::ostream& operator<<( std::ostream& os, const Value& );
	std::ostream& operator<<( std::ostream& os, ValuePtr );
}

#endif /* __VALUE_IO_HPP__ */

