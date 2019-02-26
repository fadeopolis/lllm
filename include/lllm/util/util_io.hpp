#ifndef __UTIL_IO_HPP__
#define __UTIL_IO_HPP__ 1

#include "lllm/util/InternedString.hpp"
#include "lllm/util/SourceLocation.hpp"
#include "lllm/util/TypeSet.hpp"

#include <iosfwd>

namespace lllm {
	std::ostream& operator<<( std::ostream&, const util::InternedString& );
	std::ostream& operator<<( std::ostream&, const util::SourceLocation& );
	std::ostream& operator<<( std::ostream&, const util::TypeSet& );
};

#endif /* __UTIL_IO_HPP__ */
