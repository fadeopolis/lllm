
#include "lllm/Evaluator.hpp"
#include "lllm/Jit.hpp"
#include "lllm/ast/AstIO.hpp"
#include "lllm/value/ValueIO.hpp"
#include "lllm/util/fail.hpp"
#include "lllm/util/util_io.hpp"

#include <cassert>

#if LLLM_DBG_LVL > 4
#	include <iostream>
#	define DBG( TYPE ) (void) ({ std::cout << "{VISITING " << #TYPE << "}" << std::endl; nullptr; })
#else 
#	define DBG( TYPE ) (void) ({ nullptr; })
#endif

using namespace lllm;
using namespace lllm::value;
using namespace lllm::util;

size_t Evaluator::jittingThreshold = 1000;

void Evaluator::setJittingThreshold( size_t threshold ) {
	jittingThreshold = threshold;
}

class EvalScope : public util::Scope<value::ValuePtr> {
	public:
		EvalScope( const util::InternedString& name, value::ValuePtr val, util::ScopePtr<value::ValuePtr> parent = nullptr );

		bool lookup( const util::InternedString& name, value::ValuePtr* dst ) override final;
		bool contains( const util::InternedString& name ) override final;
	private:
		const util::ScopePtr<value::ValuePtr> parent;
		util::InternedString                  name;
		value::ValuePtr                       val;
};

EvalScope::EvalScope( const util::InternedString& name, value::ValuePtr val, ScopePtr<value::ValuePtr> parent ) :
  parent( parent ),
  name( name ),
  val( val ) {}

bool EvalScope::lookup( const util::InternedString& name, value::ValuePtr* dst ) {
	if ( this->name == name ) {
		*dst = val;
		return true;
	} if ( parent ) {
		return parent->lookup( name, dst );
	} else {
		return false;
	}
}
bool EvalScope::contains( const util::InternedString& name ) {
	if ( this->name == name ) {
		return true;
	} if ( parent ) {
		return parent->contains( name );
	} else {
		return false;
	}
}

namespace lllm {
template<typename T>
static std::ostream& operator<<( std::ostream& os, const std::vector<T>& v ) {
	if ( v.size() ) return os;

	os << v.front();

	for ( auto it = ++(v.begin()), end = v.end(); it != end; ++it ) {
		os << ", " << (*it);
	}

	return os;
}
}

ValuePtr Evaluator::evaluate( ast::AstPtr ast, const util::ScopePtr<value::ValuePtr> env ) {
//std::cout << "EVAL" << std::endl;
//std::cout << ast << std::endl;
	struct Visitor {
		// ***** ATOMS
		ValuePtr visit( ast::NilPtr         ast, util::ScopePtr<value::ValuePtr> env ) const {
			return nil;
		}
		ValuePtr visit( ast::IntPtr         ast, util::ScopePtr<value::ValuePtr> env ) const {
			return number( ast->value );
		}		
		ValuePtr visit( ast::RealPtr        ast, util::ScopePtr<value::ValuePtr> env ) const {
			return number( ast->value );
		}
		ValuePtr visit( ast::CharPtr        ast, util::ScopePtr<value::ValuePtr> env ) const {
			return character( ast->value );
		}
		ValuePtr visit( ast::StringPtr      ast, util::ScopePtr<value::ValuePtr> env ) const {
			return string( ast->value );
		}
		ValuePtr visit( ast::VariablePtr    ast, util::ScopePtr<value::ValuePtr> env ) const {
			ValuePtr val;
			if ( env->lookup( ast->name, &val ) ) {
				return val;
			} else {
				LLLM_FAIL( ast->location << ": Unknown variable " << ast->name );
			}
		}	
		// ***** SPECIAL FORMS
		ValuePtr visit( ast::QuotePtr    ast, util::ScopePtr<value::ValuePtr> env ) const {
			return ast->value;
		}		
		ValuePtr visit( ast::IfPtr          ast, util::ScopePtr<value::ValuePtr> env ) const {
			if ( evaluate( ast->test, env ) ) {
				return evaluate( ast->thenBranch, env );
			} else {
				return evaluate( ast->elseBranch, env );
			}
		}
		ValuePtr visit( ast::DoPtr          ast, util::ScopePtr<value::ValuePtr> env ) const {
			ValuePtr val;
			for ( auto it = ast->exprs.begin(), end = ast->exprs.end(); it != end; ++it ) {
				val = evaluate( *it, env );
			}
			return val;
		}
		ValuePtr visit( ast::LetPtr         ast, util::ScopePtr<value::ValuePtr> env ) const {
			for ( auto it = ast->bindings.begin(), end = ast->bindings.end(); it != end; ++it ) {
				const ast::Let::Binding& b = *it;
				ValuePtr val = evaluate( b.second, env );

				env = new EvalScope( b.first, val, env );
			}
			return evaluate( ast->body, env );
		}	
		ValuePtr visit( ast::LambdaPtr      ast, util::ScopePtr<value::ValuePtr> env ) const {
			Lambda* clojure = Lambda::alloc( ast );

			size_t i = 0;
			for ( auto it = ast->capturedVariables.begin(), end = ast->capturedVariables.end(); it != end; ++it, ++i ) {
				ValuePtr val;
				if ( env->lookup( (*it)->name, &val ) ) {
					clojure->env[i] = val;
				} else {
					LLLM_FAIL( (*it)->location << ": Unknown variable " << (*it)->name );
				}
			}

			return clojure;
		}
		ValuePtr visit( ast::DefinePtr      ast, util::ScopePtr<value::ValuePtr> env ) const {
			return evaluate( ast->expr, env );
		}
		// ***** FUNCTION APPLICATION
		ValuePtr visit( ast::ApplicationPtr ast, util::ScopePtr<value::ValuePtr> env ) const {
			ValuePtr head = evaluate( ast->fun, env );

			size_t arity = ast->args.size();

			if ( LambdaPtr fun = Value::asLambda( head, arity ) ) {
				// evaluate args
				std::vector<ValuePtr> evaluatedArgs;
				evaluatedArgs.resize( ast->args.size() );
				
				size_t i = 0;
				for ( auto it = ast->args.begin(), end = ast->args.end(); it != end; ++it, ++i ) {
					evaluatedArgs[i] = evaluate( *it, env );
				}

				Lambda::FnPtr code = fun->code;

				// apply function to args
				if ( code ) {
					return applyFun( fun, arity, code, evaluatedArgs );
				} else if ( fun->data->code ) {
					code      = fun->data->code;
					fun->code = code;

					return applyFun( fun, arity, code, evaluatedArgs );
				} else {
					return applyAST( fun, env, evaluatedArgs );
				}
			} else {
				if ( typeOf( head ) < value::Type::Lambda ) {
					LLLM_FAIL( ast->location << ": Cannot apply '" << head << "' to (" << ast->args << "), it is not a function" );
				} else {
					LLLM_FAIL( ast->location << ": Cannot apply '" << head << "' to (" << ast->args << "), wrong arity" );
				}
			}
		}	
	};

	return ast->visit<ValuePtr>( Visitor(), env );
//	return nullptr;
}

ValuePtr Evaluator::applyFun( LambdaPtr fn, size_t arity, Lambda::FnPtr code, const std::vector<ValuePtr>& args ) {
//	std::cout << "APPLYING CODE " << fn->data->ast << " TO ";
//	for ( auto it = args.begin(), end = args.end(); it != end; ++it ) {
//		std::cout << *it << " ";
//	}
//	std::cout << std::endl;

	switch ( arity ) {
		case 0:  return code( fn );
	#define V    ValuePtr
	#define L    LambdaPtr
	#define A(I) args[I]
		case  1:  return ((V(*)(L,V))          code)( fn, A(0) );
		case  2:  return ((V(*)(L,V,V))        code)( fn, A(0), A(1) );
		case  3:  return ((V(*)(L,V,V,V))      code)( fn, A(0), A(1), A(2) );
		case  4:  return ((V(*)(L,V,V,V,V))    code)( fn, A(0), A(1), A(2), A(3) );
		case  5:  return ((V(*)(L,V,V,V,V,V))  code)( fn, A(0), A(1), A(2), A(3), A(4) );
		case  6:  return ((V(*)(L,V,V,V,V,V,V))code)( fn, A(0), A(1), A(2), A(3), A(4), A(5) );
	#undef V
	#undef L
	#undef A
		default: LLLM_FAIL( fn->data->ast->location << ": The interpreter cannot apply functions of arity " << arity );
	}
}

ValuePtr Evaluator::applyAST( LambdaPtr fn, util::ScopePtr<value::ValuePtr> env, const std::vector<ValuePtr>& args ) {
	Lambda::Data*  data = fn->data;
	ast::LambdaPtr ast  = data->ast;

	data->callCnt++;

	if ( data->callCnt > jittingThreshold ) {
		Jit::compile( fn, env );
		assert( fn->data->code );
		fn->code = fn->data->code;

		return applyFun( fn, args.size(), fn->code, args );
	}

	assert( ast && "A lambda that gets interpreted MUST have an AST" );

	size_t i;

	// add captured variables from clojure to env
	const std::vector<ast::VariablePtr>& capturedVars = ast->capturedVariables;

	i = 0;
	for ( auto it = capturedVars.begin(), end = capturedVars.end(); it != end; ++it, ++i ) {
		env = new EvalScope( (*it)->name, fn->env[i], env );
	}

	// add self to env (for recursion)
	if ( ast->name ) {
		env = new EvalScope( ast->name, fn, env );
	}

	// add args to env
	const std::vector<ast::VariablePtr>& parameters = ast->parameters;

	i = 0;
	for ( auto it = parameters.begin(), end = parameters.end(); it != end; ++it, ++i ) {
		env = new EvalScope( (*it)->name, args[i], env );
	}		

//	std::cout << "APPLYING AST " << fn->data->ast << " TO ";
//	for ( auto it = args.begin(), end = args.end(); it != end; ++it ) {
//		std::cout << *it << " ";
//	}
//	std::cout << std::endl;

	// eval body
	return evaluate( ast->body, env );
}

