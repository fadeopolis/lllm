
#include "lllm/util/InternedString.hpp"

#include <set>
#include <cstring>

using namespace std;
using namespace lllm;
using namespace lllm::util;

namespace {
	struct CmpStr final {
		bool operator()( CStr a, CStr b) { 
			return std::strcmp( a, b ) < 0;
		}
	};

	typedef std::set<CStr, CmpStr> InternTable;

	static InternTable* intern_table;
};

CStr InternedString::intern( CStr str ) {
	if ( !str ) return str;

	if ( !intern_table ) intern_table = new InternTable();

	InternTable& tmp = *intern_table;

	auto lb = tmp.lower_bound( str );

	if ( lb != tmp.end() && (std::strcmp( str, *lb ) == 0) ) {
		// symbol already exists
		return *lb;
	} else {
		// the symbol does not exist in the map
		// add it to the map using lb as a hint to insert, so it can avoid another lookup
		tmp.insert( lb, str );

		return str;
	}
}

