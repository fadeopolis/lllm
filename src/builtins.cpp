
#include "lllm/builtins.hpp"
#include "lllm/eval.hpp"
#include "lllm/eval/Env.hpp"
#include "lllm/analyzer/Ast.hpp"
#include "lllm/values/ValueIO.hpp"
#include "lllm/util/fail.hpp"

using namespace lllm;
using namespace lllm::eval;
using namespace lllm::value;

static reader::SourceLocation builtin_location("builtin");
//static std::map<util::InternedString, analyzer::VariablePtr> _scope;
//static eval::EnvPtr                                          _env;

BuiltinScope BuiltinScope::builtins;

analyzer::VariablePtr BuiltinScope::get( const util::InternedString& name ) {
//	return _scope[name];
	return nullptr;
}
		
EnvPtr BuiltinScope::env() const {
//	return _env;
	return nullptr;
}

BuiltinScope::BuiltinScope() {
	
}

void BuiltinScope::add( const util::InternedString& name, value::ValuePtr val, analyzer::VariablePtr ast ) {
//	_scope[name] = ast;
//	_env         = _env->put( name, val );
}

inline LambdaPtr mk_builtin_fn( void* fn, size_t arity ) {
	analyzer::Lambda::VarList params;

	for ( size_t i = 0; i < arity; i++ ) {
		params.push_back( new analyzer::Variable( builtin_location, nullptr, nullptr, analyzer::Variable::EXTERN ) );
	}

	analyzer::Lambda* ast = new analyzer::Lambda( builtin_location, nullptr, params, analyzer::Lambda::VarList(), nullptr );

	Lambda* clojure = Lambda::alloc( ast->arity(), 0 );
	
	clojure->fun = (Lambda::FnPtr) fn;
	clojure->ast = ast;

	return clojure;
}

static ValuePtr builtin_car( LambdaPtr fn, ValuePtr v ) {
	if ( ConsPtr cons = Value::asCons( v ) ) {
		return cons->car;
	} else {
		LLLM_FAIL( "builtin function 'car' expects a cons a first argument, not a " << v );
	}
}
static ValuePtr builtin_cdr( LambdaPtr fn, ValuePtr v ) {
	if ( ConsPtr cons = Value::asCons( v ) ) {
		return cons->cdr;
	} else {
		LLLM_FAIL( "builtin function 'cdr' expects a cons a first argument, not a " << v );
	}
}

EnvPtr lllm::builtins = 
	Env::make( "nil", value::nil() )
	->put( "true",    value::symbol("true") )
	->put( "false",   value::nil() )
	->put( "car",     mk_builtin_fn( (void*) builtin_car, 1 ) )
	->put( "cdr",     mk_builtin_fn( (void*) builtin_cdr, 1 ) )
;














