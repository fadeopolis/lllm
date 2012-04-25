
#include "reader/SourceLocation.hpp"

#include <iostream>

std::ostream& lllm::operator<<( std::ostream& os, const lllm::SourceLocation& sl ) {
	return os << sl.file() << ':' << sl.line() << ':' << sl.column();
}

