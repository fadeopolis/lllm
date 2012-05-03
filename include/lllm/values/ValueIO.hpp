#ifndef __VALUE_IO_HPP__
#define __VALUE_IO_HPP__

#include "lllm/values/Value.hpp"

#include <iosfwd>

namespace lllm {
	namespace value {
		std::ostream& operator<<( std::ostream&, const Value::Type& );	
		std::ostream& operator<<( std::ostream&, const Value& );	
		std::ostream& operator<<( std::ostream&, const Value* );	

	}; // end namespace value
}; // end namespace lllm

#endif /* __VALUE_IO_HPP__ */

