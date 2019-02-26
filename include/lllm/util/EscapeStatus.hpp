#ifndef __LLLM_UTIL_ESCAPE_STATUS_HPP__
#define __LLLM_UTIL_ESCAPE_STATUS_HPP__ 1

namespace lllm {
	namespace util {
		enum class EscapeStatus {
			NO_ESCAPE,
			ESCAPE_AS_PARAM,
			ESCAPE_AS_RETURN,
			ESCAPE_GLOBAL
		};

		EscapeStatus max( EscapeStatus, EscapeStatus );
	};
};

#endif /* __LLLM_UTIL_ESCAPE_STATUS_HPP__ */
