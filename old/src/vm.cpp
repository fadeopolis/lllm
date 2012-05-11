
#include "lllm/vm.hpp"
#include "lllm/builtins.hpp"
#include "lllm/eval.hpp"
#include "lllm/eval/Env.hpp"

using namespace lllm;
using namespace lllm::analyzer;

VM::VM() : _scope( nullptr ) {
	
}

VM* VM::make() { return new VM(); }

eval::EnvPtr             VM::env() {
	return _env;
}
analyzer::GlobalScopePtr VM::scope() {
	return &_scope;
}

