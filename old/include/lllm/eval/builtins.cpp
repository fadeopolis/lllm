
#include "lllm/eval.hpp"
#include "lllm/eval/Env.hpp"
#include "lllm/analyzer/Ast.hpp"

using namespace lllm;
using namespace lllm::eval;
using namespace lllm::value;

static SourceLocation builtin_location("builtin");

static inline analyzer::LambdaPtr mk_builtin_ast( analyzer::Lambda::ParameterList& params ) {
	return new analyzer::Lambda( builtin_location, nullptr, params, analyzer::Lambda::CaptureList(), nullptr );
}

template<typename... Args>
static inline analyzer::LambdaPtr mk_builtin_ast( Args... args, CStr arg, analyzer::Lambda::ParameterList& params ) {
	params.push_back( new analyzer::Parameter( builtin_location, arg ) );

	return mk_builtin_ast( args..., params );
}

template<typename... Args>
static inline LambdaPtr mk_builtin_fn( void* fn, Args... params ) {
	analyzer::Lambda::ParameterList tmp;
	analyzer::LambdaPtr ast = mk_builtin_ast( params..., tmp );

	Lambda* clojure = Lambda::alloc( ast->arity(), 0 );
	
	clojure->fun = (Lambda::FnPtr) fn;
	clojure->ast = ast;

	return clojure;
}

static ValuePtr builtin_car( LambdaPtr fn, ValuePtr cons ) {
	return nullptr;
}

EnvPtr lllm::builtins = 
	Env::make( "nil", value::nil() )
	->put( "true",    value::symbol("true") )
	->put( "false",   value::nil() )
	->put( "car",     mk_builtin_fn( builtin_car, "cons" ) )
;




