#ifndef __EVAL_HPP__
#define __EVAL_HPP__ 1

#include "Value.hpp"
#include "Env.hpp"

namespace lllm {
	EnvPtr builtins();

	ValuePtr eval( ValuePtr v, EnvPtr env );
}

#endif /* __EVAL_HPP__ */


