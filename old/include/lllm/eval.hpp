#ifndef __EVAL_HPP__
#define __EVAL_HPP__

#include "lllm/predef.hpp"
#include "lllm/eval/Env.hpp"

namespace lllm {
	value::ValuePtr evaluate( analyzer::ConstAstPtr ast, eval::EnvPtr env );

	extern eval::EnvPtr builtins;
};

#endif /* __EVAL_HPP__ */

