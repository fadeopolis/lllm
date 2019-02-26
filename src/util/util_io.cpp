
#include "lllm/util/util_io.hpp"

#include <iostream>

using namespace std;
using namespace lllm;
using namespace lllm::util;

std::ostream& lllm::operator<<( std::ostream& os, const InternedString& str ) {
	return os << ((CStr) str);
}
std::ostream& lllm::operator<<( std::ostream& os, const SourceLocation& sl ) {
	return os << sl.file() << ':' << sl.line() << ':' << sl.column();
}

static inline void printType( std::ostream& os, value::Type t ) {
	switch ( t ) {
		#define LLLM_VISIT( TYPE )
		#define LLLM_VISIT_CONCRETE( TYPE ) case value::Type::TYPE: os << #TYPE; break;
		#include "lllm/value/Value.inc"

		default: os << "Lambda"; break;
	}
}

std::ostream& lllm::operator<<( std::ostream& os, const TypeSet& ts ) {
	os << '{';

	bool flag = false;

	for ( value::Type t = value::Type::BEGIN; t <= value::Type::END; t = value::Type( size_t( t ) + 1 ) ) {
		if ( ts.contains( t ) ) {
			if ( flag ) os << ", ";
			printType( os, t );
			flag = true;
		}
	}

	return os << '}';
}
