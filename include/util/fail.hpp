#ifndef __FAIL_HPP__
#define __FAIL_HPP__ 1

#include <sstream>

namespace lllm {
	// print message, then print a stack trace, then abort
	void fail( const char* file, const char* function, int line, const char* msg ) __attribute__((noreturn));

	#define LLLM_FAIL( MSG ) ({ \
		std::stringstream str; \
		str << MSG; \
		lllm::fail( __FILE__, __PRETTY_FUNCTION__, __LINE__, str.str().c_str() ); \
	})
}

#endif /* __FAIL_HPP__ */

