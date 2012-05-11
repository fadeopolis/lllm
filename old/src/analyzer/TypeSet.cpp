
#include "lllm/analyzer/TypeSet.hpp"

#include <iostream>

using namespace lllm;
using namespace lllm::value;

std::ostream& lllm::operator<<( std::ostream& os, const TypeSet& ts ) {
	if ( !ts.mask ) {
		return os << "{}";
	} else {
		os << '{';

		bool flag = false;

		for ( Value::Type t = Value::BEGIN; t <= Value::END; t = Value::Type( long( t ) + 1 ) ) {
			if ( ts.contains( t ) ) {
				if ( flag ) os << ", ";
				os << t;
				flag = true;
			}
		}

		return os << '}';
	}
}

