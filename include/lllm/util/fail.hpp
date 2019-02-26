#ifndef __FAIL_HPP__
#define __FAIL_HPP__ 1

#include <sstream>

namespace lllm {
		namespace util {
		// print message, then print a stack trace, then abort
		void fail( const char* file, const char* function, int line, const char* msg ) __attribute__((noreturn));

		#define LLLM_FAIL( MSG ) ({ \
			::std::stringstream _str_; \
			_str_ << MSG; \
			::lllm::util::fail( __FILE__, __PRETTY_FUNCTION__, __LINE__, _str_.str().c_str() ); \
		})
	}; // end namespace util
}

#endif /* __FAIL_HPP__ */
