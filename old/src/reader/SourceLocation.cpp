
#include "lllm/reader/SourceLocation.hpp"

#include <iostream>

using namespace lllm;
using namespace lllm::reader;

std::ostream& reader::operator<<( std::ostream& os, const SourceLocation& sl ) {
	return os << sl.file() << ':' << sl.line() << ':' << sl.column();
}

