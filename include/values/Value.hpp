#ifndef __VALUE_HPP__
#define __VALUE_HPP__ 1

#include <iosfwd>

namespace lllm {
	enum class Type : long {
		Nil,
		Cons,
		Int,
		Real,
		Char,
		String,
		Symbol,
		Ref,
		Lambda,
		Thunk,
		// markers
		BEGIN = Nil,
		END   = Thunk
	};

	std::ostream& operator<<( std::ostream&, const Type& );	
};


#endif /* __VALUE_HPP__ */

