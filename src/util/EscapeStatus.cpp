
#include "lllm/util/EscapeStatus.hpp"

using namespace lllm;
using namespace lllm::util;

EscapeStatus util::max( EscapeStatus a, EscapeStatus b ) {
	return (a) > (b) ? a : b;
}
