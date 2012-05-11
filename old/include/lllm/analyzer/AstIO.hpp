#ifndef __AST_IO_HPP__
#define __AST_IO_HPP__

#include <iosfwd>

#include "lllm/analyzer/Ast.hpp"

namespace lllm {
	namespace analyzer {
		std::ostream& operator<<( std::ostream&, ConstAstPtr );
	}; // end namespace analyzetr
}; // end namespace lllm

#endif /* __AST_IO_HPP__ */

