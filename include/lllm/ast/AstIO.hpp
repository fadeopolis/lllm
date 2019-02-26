#ifndef __Ast_IO_HPP__
#define __Ast_IO_HPP__ 1

#include "lllm/ast/Ast.hpp"

#include <iosfwd>

namespace lllm {
	std::ostream& operator<<( std::ostream&, ast::ConstAstPtr );
};

#endif /* __Ast_IO_HPP__ */
