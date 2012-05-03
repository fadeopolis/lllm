
#include "lllm/eval.hpp"
#include "lllm/eval/Env.hpp"
#include "lllm/analyzer/Ast.hpp"
#include "lllm/values/Value.hpp"
#include "lllm/values/ValueIO.hpp"
#include "lllm/util/fail.hpp"

#include <cassert>

using namespace lllm;
using namespace lllm::eval;
using namespace lllm::value;

static ValuePtr applyAST( LambdaPtr fn, EnvPtr env, const std::vector<ValuePtr>& args ) {
	assert( fn->ast && "A lambda MUST have an AST" );

	size_t i;

	// add captured variables from clojure to env
	const analyzer::Lambda::VarList& capturedVars = fn->ast->capturedVariables;

	i = 0;
	for ( auto it = capturedVars.begin(), end = capturedVars.end(); it != end; ++it, ++i ) {
		env = env->put( (*it)->name, fn->env[i] );
	}

	// add args to env
	const analyzer::Lambda::VarList& parameters = fn->ast->parameters;

	i = 0;
	for ( auto it = parameters.begin(), end = parameters.end(); it != end; ++it, ++i ) {
		env = env->put( (*it)->name, args[i] );
	}		

	// eval body
	return evaluate( fn->ast, env );
}

ValuePtr lllm::evaluate( analyzer::ConstAstPtr ast, EnvPtr env ) {
	struct Visitor {
		ValuePtr visit( analyzer::ConstAstPtr ast, EnvPtr env ) const {
			return nullptr;
		}
		// ***** ATOMS
		ValuePtr visit( analyzer::ConstNilPtr    ast, EnvPtr env ) const {
			return nil();
		}
		ValuePtr visit( analyzer::ConstIntPtr    ast, EnvPtr env ) const {
			return number( ast->value );
		}		
		ValuePtr visit( analyzer::ConstRealPtr   ast, EnvPtr env ) const {
			return number( ast->value );
		}
		ValuePtr visit( analyzer::ConstCharPtr   ast, EnvPtr env ) const {
			return character( ast->value );
		}
		ValuePtr visit( analyzer::ConstStringPtr ast, EnvPtr env ) const {
			return string( ast->value );
		}
		ValuePtr visit( analyzer::ConstVariablePtr ast, EnvPtr env ) const {
			return env->get( ast->name );
		}	
		// ***** SPECIAL FORMS
		ValuePtr visit( analyzer::ConstQuotePtr ast, EnvPtr env ) const {
			return ast->value;
		}		
		ValuePtr visit( analyzer::ConstIfPtr ast, EnvPtr env ) const {
			if ( evaluate( ast->test, env ) ) {
				return evaluate( ast->thenBranch, env );
			} else {
				return evaluate( ast->elseBranch, env );
			}
		}
		ValuePtr visit( analyzer::ConstDoPtr ast, EnvPtr env ) const {
			ValuePtr val;
			for ( auto it = ast->exprs.begin(), end = ast->exprs.end(); it != end; ++it ) {
				val = evaluate( *it, env );
			}
			return val;
		}
		ValuePtr visit( analyzer::ConstLetPtr ast, EnvPtr env ) const {
			for ( auto it = ast->bindings.begin(), end = ast->bindings.end(); it != end; ++it ) {
				analyzer::ConstVariablePtr var = *it;

				ValuePtr val = evaluate( var, env );

				env = env->put( var->name, val );
			}

			return evaluate( ast->expr, env );
		}	
		ValuePtr visit( analyzer::ConstLambdaPtr ast, EnvPtr env ) const {
			Lambda* clojure = Lambda::alloc( ast->arity(), ast->capturedVariables.size() );

			clojure->ast = ast;

			size_t i = 0;
			for ( auto it = ast->capturedVariables.begin(), end = ast->capturedVariables.end(); it != end; ++it, ++i ) {
				analyzer::ConstVariablePtr var = *it;

				assert( var->storage == analyzer::Variable::CAPTURED );

				clojure->env[i] = env->get( var->name );
			}

			return clojure;
		}
		// ***** FUNCTION APPLICATION
		ValuePtr visit( analyzer::ConstApplicationPtr ast, EnvPtr env ) const {
			ValuePtr head = evaluate( ast->fun, env );

			if ( LambdaPtr fun = Value::asLambda( head, ast->args.size() ) ) {
				// evaluate args
				std::vector<ValuePtr> evaluatedArgs;
				evaluatedArgs.resize( ast->args.size() );
				
				size_t i = 0;
				for ( auto it = ast->args.begin(), end = ast->args.end(); it != end; ++it, ++i ) {
					evaluatedArgs[i] = evaluate( *it, env );
				}

				// apply function to args
				if ( fun->fun ) {
					switch ( fun->arity() ) {
						default: LLLM_FAIL( ast->location << ": The interpreter cannot apply functions of arity " << fun->arity() );
					}
				} else {
					return applyAST( fun, env, evaluatedArgs );
				}
			} else {
				if ( typeOf( head ) < Value::Lambda ) {
					LLLM_FAIL( ast->location << ": Cannot apply '" << head << "', it is not a function" );
				} else {
					LLLM_FAIL( ast->location << ": Cannot apply '" << head << "', wrong arity" );
				}
			}
		}	
	};

	return ast->visit<ValuePtr>( Visitor(), env );
//	return nullptr;
}


