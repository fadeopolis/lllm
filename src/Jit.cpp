
#include "lllm/Jit.hpp"
#include "lllm/Evaluator.hpp"
#include "lllm/ast/AstIO.hpp"
#include "lllm/value/ValueIO.hpp"
#include "lllm/util/fail.hpp"
#include "lllm/util/util_io.hpp"

#include <jit/jit.h>
#include <jit/jit-dump.h>

#include <map>
#include <cassert>
#include <cstdio>
#include <iostream>

//#include <p99_for.h>

#if LLLM_DBG_LVL > 5
#	define DBG( TYPE ) (void) ({ std::cout << "{VISITING " << #TYPE << "}" << std::endl; nullptr; })
#else 
#	define DBG( TYPE ) (void) ({ nullptr; })
#endif

using namespace lllm;
using namespace lllm::value;

static_assert( sizeof(void*) == 8, "Pointers must be 8 bytes wide" );
static_assert( sizeof(long)  == 8, "Long must be 8 bytes wide" );

struct JitScope : public util::Scope<jit_value_t>, public util::Scope<jit_function_t> {
	bool lookup( const util::InternedString& name, jit_value_t*    dst ) override = 0;
	bool lookup( const util::InternedString& name, jit_function_t* dst ) override = 0;
	bool contains( const util::InternedString& name ) override                    = 0;

	virtual void dump() = 0;
};
typedef JitScope* JitScopePtr;

size_t Jit::inliningThreshold = 2;
size_t Jit::inliningDepth     = 1;

void Jit::setInliningThreshold( size_t depthThreshold ) {
	inliningThreshold = depthThreshold;
}
void Jit::setInliningDepth( size_t maxRecursion ) {
	inliningDepth = maxRecursion;
}

class ScopeAdapter final : public JitScope {
	public:
		ScopeAdapter( util::ScopePtr<value::ValuePtr> scope, jit_function_t fn ) : scope( scope ), fn( fn ) {}

		bool lookup( const util::InternedString& name, jit_value_t*   dst ) override final {
			ValuePtr val;
			if ( scope->lookup( name, &val ) ) {
				*dst = jit_value_create_long_constant( fn, jit_type_void_ptr, (long)(void*) val );
				return true;
			} else {
				return false;
			}
		}
		bool lookup( const util::InternedString& name, jit_function_t* dst ) override final {
			return false;
		}
		bool contains( const util::InternedString& name ) override final {
			return scope->contains( name );
		}

		void dump() override final {
			scope->dump();
		}
	private:
		const util::ScopePtr<value::ValuePtr> scope;
		jit_function_t                        fn;
};
class JitValueScope final : public JitScope {
	public:
		JitValueScope( const util::InternedString& name, jit_value_t val, JitScopePtr parent = nullptr ) :
		  parent( parent ),
		  name( name ),
		  val( val ) {}

		bool lookup( const util::InternedString& name, jit_value_t*    dst ) override final {
			if ( this->name == name ) {
				*dst = val;
				return true;
			} else if ( parent ) {
				return parent->lookup( name, dst );
			} else {
				return false;
			}
		}
		bool lookup( const util::InternedString& name, jit_function_t* dst ) override final {
			return parent->lookup( name, dst );
		}
		bool contains( const util::InternedString& name ) override final {
			if ( this->name == name ) {
				return true;
			} else if ( parent ) {
				return parent->contains( name );
			} else {
				return false;
			}
		}

		void dump() override final {
			std::cout << "*VAL " << name << "\t" << val << std::endl;
			if ( parent ) parent->dump();
		}
	private:
		const JitScopePtr    parent;
		util::InternedString name;
		jit_value_t          val;
};
class JitFunctionScope final : public JitScope {
	public:
		JitFunctionScope( const util::InternedString& name, jit_function_t fun, JitScopePtr parent = nullptr ) :
		  parent( parent ),
		  name( name ),
		  fun( fun ) {}

		bool lookup( const util::InternedString& name, jit_value_t* dst ) override final {
			return parent->lookup( name, dst );
		}
		bool lookup( const util::InternedString& name, jit_function_t* dst ) override final {
			if ( this->name == name ) {
				*dst = fun;
				return true;
			} else if ( parent ) {
				return parent->lookup( name, dst );
			} else {
				return false;
			}
		}
		bool contains( const util::InternedString& name ) override final {
			if ( this->name == name ) {
				return true;
			} else if ( parent ) {
				return parent->contains( name );
			} else {
				return false;
			}
		}

		void dump() override final {
			std::cout << "*FUN " << name << "\t" << fun << std::endl;
			if ( parent ) parent->dump();
		}
	private:
		const JitScopePtr    parent;
		util::InternedString name;
		jit_function_t       fun;
};


struct Jit::SharedData {
	SharedData();

	jit_context_t ctx;

	jit_type_t tag_t;
	jit_type_t ptr_t;

	jit_type_t value_t;
	jit_type_t cons_t;
	jit_type_t int_t;
	jit_type_t real_t;
	jit_type_t char_t;
	jit_type_t string_t;
	jit_type_t symbol_t;
	jit_type_t ref_t;

	jit_type_t fail_signature;

	std::map<size_t, jit_type_t> signature_ts;

	jit_type_t signature( size_t arity );
};

static inline int envElementOffset( int elemIdx ) {
	return 24 + (elemIdx * sizeof(ValuePtr));
}

static inline bool isTailRecursive( ast::AstPtr fun, jit_value_t env, JitScopePtr scope ) {
	if ( ast::VariablePtr var = fun->as<ast::Variable>() ) {
		jit_value_t val;
		if ( scope->lookup( var->name, &val ) && env == val ) {
			return true;
		}
	}

	return false;
}

static inline ast::LambdaPtr asLambda( ast::AstPtr ast, util::ScopePtr<value::ValuePtr> scope ) {
	if ( ast::LambdaPtr lambda = dynamic_cast<ast::LambdaPtr>( ast ) ) {
		return lambda;
	}
	if ( ast::VariablePtr var = dynamic_cast<ast::VariablePtr>( ast ) ) {
		ValuePtr val;
		if ( scope->lookup( var->name, &val ) ) {
			if ( LambdaPtr lambda = Value::asLambda( val ) ) {
				return lambda->data->ast;
			}
		}
	}

	return nullptr;
}
static inline Lambda::FnPtr getCodeOrNull( ast::AstPtr ast, util::ScopePtr<value::ValuePtr> scope ) {
	if ( ast::VariablePtr var = dynamic_cast<ast::VariablePtr>( ast ) ) {
		ValuePtr val;
		if ( scope->lookup( var->name, &val ) ) {
			if ( LambdaPtr lambda = Value::asLambda( val ) ) {
				return lambda->data->code;
			}
		}
	}

	return nullptr;
}
static inline const char* getNameOrFail( ast::AstPtr ast ) {
	if ( ast::VariablePtr var = dynamic_cast<ast::VariablePtr>( ast ) ) {
		return var->name;
	}

	LLLM_FAIL("getNameOrFail failed");
}

extern "C" {
	static void lllm_fail_apply( void* v ) {
		LLLM_FAIL( "Cannot apply " << reinterpret_cast<ValuePtr>( v ) );
	}
	static void* lllm_alloc_lambda( void* v ) {
		return Lambda::alloc( reinterpret_cast<ast::LambdaPtr>( v ) );
	}

	static void* lllm_jit( void* rawFn, void* rawEnv ) {
		auto fn    = (value::LambdaPtr)                      rawFn;
		auto env   = (const util::ScopePtr<value::ValuePtr>) rawEnv;

		Jit::compile( fn, env );

		fn->code = fn->data->code;

		return (void*) fn->code;
	}
}

void Jit::compile( value::LambdaPtr fn, util::ScopePtr<value::ValuePtr> globals ) {
	if ( !shared ) shared = new SharedData();

	// don't compile twice
	if ( fn->code ) return;

	if ( fn->data->code ) {
		fn->code = fn->data->code;
		return;
	}

	// start compiling
	jit_context_build_start( shared->ctx );
	
	ast::LambdaPtr      ast = fn->data->ast;

	std::cout << "JITTING " << ast->name << std::endl;

	// do inlining
	ast = performInlining( ast, globals );

//	printf( "JITTING %s\n", (util::CStr)ast->name );

	jit_function_t fnIr = jit_function_create( shared->ctx, shared->signature( fn->arity() ) );
	
	struct Visitor {
		jit_value_t visit( ast::NilPtr         ast, JitScopePtr scope, bool tail ) {
			DBG( Nil );
			return jit_value_create_long_constant( ir, shared->ptr_t, 0 );
		}
		jit_value_t visit( ast::IntPtr         ast, JitScopePtr scope, bool tail ) {
			DBG( Int );
			return jit_value_create_long_constant( ir, shared->ptr_t, (long)(void*) number( ast->value ) );
		}
		jit_value_t visit( ast::RealPtr        ast, JitScopePtr scope, bool tail ) {
			DBG( Real );
			return jit_value_create_long_constant( ir, shared->ptr_t, (long)(void*) number( ast->value ) );
		}
		jit_value_t visit( ast::CharPtr        ast, JitScopePtr scope, bool tail ) {
			DBG( Char );
			return jit_value_create_long_constant( ir, shared->ptr_t, (long)(void*) character( ast->value ) );
		}
		jit_value_t visit( ast::StringPtr      ast, JitScopePtr scope, bool tail ) {
			DBG( String );
			return jit_value_create_long_constant( ir, shared->ptr_t, (long)(void*) string( ast->value ) );
		}
		jit_value_t visit( ast::VariablePtr    ast, JitScopePtr scope, bool tail ) {
			DBG( Variable );
			jit_value_t val;
			if ( scope->lookup( ast->name, &val ) ) {
				return val;	
			} else {
				scope->dump();
				LLLM_FAIL( ast->location << ": Undefined variable " << ast );
			}
		}
		jit_value_t visit( ast::QuotePtr       ast, JitScopePtr scope, bool tail ) {
			DBG( Quote );
			return jit_value_create_long_constant( ir, shared->ptr_t, (long)(void*) ast->value );
		}
		jit_value_t visit( ast::IfPtr          ast, JitScopePtr scope, bool tail ) {
			DBG( If );

			jit_label_t elseLabel = jit_label_undefined;
			jit_label_t endLabel  = jit_label_undefined;

			jit_value_t result = jit_value_create( ir, shared->ptr_t );

			// emit test code
			jit_value_t test = ast->test->visit<jit_value_t>( *this, scope, false ); 
			// branch to else part if test == 0
			jit_insn_branch_if_not( ir, test, &elseLabel );    
			// emit then part
			jit_value_t thenResult = ast->thenBranch->visit<jit_value_t>( *this, scope, tail );
			if ( thenResult ) {
				jit_insn_store( ir, result, thenResult );
				jit_insn_branch( ir, &endLabel );
			} else {
				// then part was tail recursive, we never reach the code after it
			}
			// emit else part
			jit_insn_label( ir, &elseLabel );
			jit_value_t elseResult = ast->elseBranch->visit<jit_value_t>( *this, scope, tail );
			if ( elseResult ) {
				jit_insn_store( ir, result, elseResult );
			} else {
				// else part was tail recursive, we never reach the code after it
			}			
			// emit end part
			jit_insn_label( ir, &endLabel );

			return result;
		}
		jit_value_t visit( ast::DoPtr          ast, JitScopePtr scope, bool tail ) {
			DBG( Do );

			for ( auto it = ast->begin(), end = --(ast->end()); it != end; ++it ) {
				(*it)->visit<jit_value_t>( *this, scope, false );
			}

			return ast->back()->visit<jit_value_t>( *this, scope, tail );
		}
		jit_value_t visit( ast::LetPtr         ast, JitScopePtr scope, bool tail ) {
			DBG( Let );

			auto newScope = scope;

			for ( auto it = ast->begin(), end = ast->end(); it != end; ++it ) {
				const ast::Let::Binding& b = *it;

				auto name = b.first;
				auto val  = b.second->visit<jit_value_t>( *this, scope, false );

				val = jit_insn_load( ir, val );

				newScope = new JitValueScope( name, val , newScope );
			}

			return ast->body->visit<jit_value_t>( *this, newScope, tail );
		}
		jit_value_t visit( ast::LetStarPtr     ast, JitScopePtr scope, bool tail ) {
			DBG( Let );

			for ( auto it = ast->begin(), end = ast->end(); it != end; ++it ) {
				const ast::Let::Binding& b = *it;

				auto name = b.first;
				auto val  = b.second->visit<jit_value_t>( *this, scope, false );

				val = jit_insn_load( ir, val );

				scope = new JitValueScope( name, val , scope );
			}

			return ast->body->visit<jit_value_t>( *this, scope, tail );
		}
		jit_value_t visit( ast::LambdaPtr      ast, JitScopePtr scope, bool tail ) {
			DBG( Lambda );

			jit_value_t args[1];
			args[0] = jit_value_create_long_constant( ir, shared->ptr_t, (long)(void*) ast );

			// create closure
			jit_value_t lambda = jit_insn_call_native( ir, "lllm::value::Lambda::alloc", (void*)lllm_alloc_lambda, 
			                                           shared->signature( 0 ), args, 1, 0 );

			// fill env
			int idx = 0;
			for ( auto it = ast->capture_begin(), end = ast->capture_end(); it != end; ++it, ++idx ) {
				jit_value_t cap;
				if ( scope->lookup( (*it)->name, &cap ) ) {
					jit_insn_store_relative( ir, lambda, envElementOffset( idx ), cap );
				} else {
					LLLM_FAIL( (*it)->location << ": Unknown variable " << (*it)->name );
				}
			}

			return lambda;
		}
		jit_value_t visit( ast::ApplicationPtr ast, JitScopePtr scope, bool tail ) {
			DBG( Application );

			if ( ast::LambdaPtr lambda = asLambda( ast->fun, globals ) ) {
				return emitCallToConstant( ast, lambda, scope, tail );
			} else {
				return emitNormalCall( ast, scope, tail );
			}
		}

		jit_value_t emitCallToConstant( ast::ApplicationPtr ast, ast::LambdaPtr fn, JitScopePtr scope, bool tail ) {
			size_t arity = ast->arity();

			if ( arity != fn->arity() ) LLLM_FAIL("Cannot apply " << fn << " to " << arity << " arguments");
	
			// emit code for function to apply
			jit_value_t  fun  = ast->fun->visit<jit_value_t>( *this, scope, false );

			// emit code for args
			jit_value_t* args = new jit_value_t[ast->args.size() + 2];
		
			args[0] = fun;

			int idx = 1;
			for ( auto it = ast->begin(), end = ast->end(); it != end; ++it, ++idx ) {
				args[idx] = (*it)->visit<jit_value_t>( *this, scope, false );
			}

			args[idx] = jit_value_create_long_constant( ir, shared->ptr_t, (long)(void*) globals );

			// emit code for call
			if ( fun == self ) {
				if ( tail ) {
					// ** emit tail recursive call
					jit_value_t v = jit_insn_tail_call( ir, name, args, arity + 1, fnEntry );
					assert( v && "jit_insn_tail_call() failed!" );
					return nullptr;
				} else {
					// ** emit normal recursive call
					return jit_insn_call( ir, name, ir, nullptr, args, arity + 1, 0 );
				}
			} else {
				// ** emit normal call

				if ( Lambda::FnPtr codePtr = getCodeOrNull( ast->fun, globals ) ) {
					// function constant, code can never be null, emit call
					return jit_insn_call_native( ir, getNameOrFail( ast->fun ), (void*) codePtr, shared->signature( arity ), args, arity + 1, 0 );
				} else {
					jit_label_t fnIsNotCompiled = jit_label_undefined;
					jit_label_t end             = jit_label_undefined;

					jit_value_t result = jit_value_create( ir, shared->ptr_t );

					// get code ptr for call
					jit_value_t code = jit_insn_load_relative( ir, fun , 8, shared->ptr_t );
					// check code for null
					jit_insn_branch_if_not( ir, code, &fnIsNotCompiled );
					// code is not null, emit call
					jit_value_t callResult = jit_insn_call_indirect( ir, code, shared->signature( arity ), args, arity + 1, 0 );
					jit_insn_store( ir, result, callResult );
					jit_insn_branch( ir, &end );
					// jit uncompiled function, then call it
					jit_insn_label( ir, &fnIsNotCompiled );
					jit_value_t jitArgs[] = { fun, jit_value_create_long_constant( ir, shared->ptr_t, (long)(void*)globals ) };
					jit_value_t newCode   = jit_insn_call_native( ir, "lllm_jit", (void*)lllm_jit, shared->signature(1), jitArgs, 2, 0 );
					jit_value_t tmp       = jit_insn_call_indirect( ir, newCode, shared->signature( arity ), args, arity + 1, 0 );
					jit_insn_store( ir, result, tmp );
					jit_insn_branch( ir, &end );
		
					// done
					jit_insn_label( ir, &end );
					return result;
				}
			}
		}
		jit_value_t emitNormalCall( ast::ApplicationPtr ast, JitScopePtr scope, bool tail ) {
			size_t arity = ast->arity();
	
			// emit code for function to apply
			jit_value_t  fun  = ast->fun->visit<jit_value_t>( *this, scope, false );

			// emit code for args
			jit_value_t* args = new jit_value_t[ast->args.size() + 2];
		
			args[0] = fun;

			int idx = 1;
			for ( auto it = ast->begin(), end = ast->end(); it != end; ++it, ++idx ) {
				args[idx] = (*it)->visit<jit_value_t>( *this, scope, false );
			}

			args[idx] = jit_value_create_long_constant( ir, shared->ptr_t, (long)(void*) globals );

			// emit code for call
			if ( fun == self ) {
				if ( tail ) {
					// ** emit tail recursive call
					jit_value_t v = jit_insn_tail_call( ir, name, args, arity + 1, fnEntry );
					assert( v && "jit_insn_tail_call() failed!" );
					return nullptr;
				} else {
					// ** emit normal recursive call
					return jit_insn_call( ir, name, ir, nullptr, args, arity + 1, 0 );
				}
			} else {
				// ** emit normal call
				jit_label_t fnIsNull        = jit_label_undefined;
				jit_label_t fnHasWrongArity = jit_label_undefined;
				jit_label_t fnIsNotCompiled = jit_label_undefined;
				jit_label_t end             = jit_label_undefined;

				jit_value_t result = jit_value_create( ir, shared->ptr_t );

				// check for null
				jit_insn_branch_if_not( ir, fun, &fnIsNull );
				// type & arity check
				jit_value_t typeTag     = jit_insn_load_relative( ir, fun, 0, shared->tag_t );
				jit_value_t expectedTag = jit_value_create_long_constant( ir, shared->tag_t, size_t(value::Type::Lambda) + arity );
				jit_value_t arityCheck  = jit_insn_eq( ir, typeTag, expectedTag );
				jit_insn_branch_if_not( ir, arityCheck, &fnHasWrongArity );
				// get code ptr for call
				jit_value_t code   = jit_insn_load_relative( ir, fun , 8, shared->ptr_t );
				// check code for null
				jit_insn_branch_if_not( ir, code, &fnIsNotCompiled );
				// code is not null, emit call
				jit_value_t callResult = jit_insn_call_indirect( ir, code, shared->signature( arity ), args, arity + 1, 0 );
				jit_insn_store( ir, result, callResult );
				jit_insn_branch( ir, &end );
				// jit uncompiled function, then call it
				jit_insn_label( ir, &fnIsNotCompiled );
				jit_value_t jitArgs[] = { fun, jit_value_create_long_constant( ir, shared->ptr_t, (long)(void*)globals ) };
				jit_value_t newCode   = jit_insn_call_native( ir, "lllm_jit", (void*)lllm_jit, shared->signature(1), jitArgs, 2, 0 );
				jit_value_t tmp       = jit_insn_call_indirect( ir, newCode, shared->signature( arity ), args, arity + 1, 0 );
				jit_insn_store( ir, result, tmp );
				jit_insn_branch( ir, &end );
				// emit null check fail
				jit_insn_label( ir, &fnIsNull );
				jit_insn_call_native( ir, "lllm_fail_apply", (void*)lllm_fail_apply, shared->fail_signature, args, 1, JIT_CALL_NORETURN );			
				// emit arity check fail
				jit_insn_label( ir, &fnHasWrongArity );
				jit_insn_call_native( ir, "lllm_fail_apply", (void*)lllm_fail_apply, shared->fail_signature, args, 1, JIT_CALL_NORETURN );			
	
				// done
				jit_insn_label( ir, &end );
				return result;
			}
		}
		jit_value_t visit( ast::DefinePtr      ast, JitScopePtr scope, bool tail ) {
			DBG( Define );
			LLLM_FAIL( ast->location << ": Define statements may not appear within a function" );
		}

		util::CStr      name;
		jit_function_t  ir;
		jit_value_t     self;
		jit_value_t     env;
		jit_label_t*    fnEntry;

		util::ScopePtr<value::ValuePtr> globals;
	};

	JitScopePtr scope = new ScopeAdapter( globals, fnIr );

	int idx;

	jit_value_t self = jit_value_create_long_constant( fnIr, shared->ptr_t, (long)(void*) fn );

	// add captured vars to scope
	jit_value_t env = jit_value_get_param( fnIr, 0 );

	idx = 0;
	for ( auto it = ast->capture_begin(), end = ast->capture_end(); it != end; ++it, ++idx ) {
		jit_value_t envElem = jit_insn_load_relative( fnIr, env, envElementOffset( idx ), shared->ptr_t );

		scope = new JitValueScope( (*it)->name, envElem, scope );
	}

	// add self for tail recursion
	if ( std::strcmp( ast->name, "" ) != 0 ) {
		scope = new JitValueScope( ast->name, self, scope );
	}

	// add params to scope
	idx = 1;
	for ( auto it = ast->params_begin(), end = ast->params_end(); it != end; ++it, ++idx ) {
		scope = new JitValueScope( (*it)->name, jit_value_get_param( fnIr, idx ), scope );
	}	

	// create label for tail recursion hack
	jit_label_t fnEntry = jit_label_undefined;
	jit_insn_label( fnIr, &fnEntry );

	// create libjit ir
	Visitor v{ (util::CStr)ast->name, fnIr, self, env, &fnEntry, globals };
	jit_value_t retVal = ast->body->visit<jit_value_t>( v, scope, true );
	if ( retVal ) jit_insn_return( fnIr, retVal );

//	std::printf("-- ABOUT TO COMPILE %s ------------------------------------\n", (util::CStr)ast->name );
//	jit_dump_function( stderr, fnIr, ast->name );
//	std::printf("-----------------------------------------------------------\n");

	if ( jit_function_compile( fnIr ) == 0 ) {
		LLLM_FAIL( ast->location << "Could not compile function " << ast );
	}

//	std::printf("-- COMPILED %s --------------------------------------------\n", (util::CStr)ast->name );
//	jit_dump_function( stderr, fnIr, ast->name );
// 		std::printf("-----------------------------------------------------------\n");

	jit_context_build_end( shared->ctx );

	fn->data->code = (Lambda::FnPtr) jit_function_to_closure( fnIr );

//	printf(">> %p\n", fn->data->code );
}



ast::LambdaPtr Jit::performInlining( ast::LambdaPtr fn, util::ScopePtr<value::ValuePtr> globals ) {
//	std::cout << "INLINING " << (util::CStr)fn->name << std::endl;

	struct Visitor {
		ast::AstPtr visit( ast::AstPtr         ast, util::ScopePtr<value::ValuePtr> globals ) const {
			//std::cout << "BORING " << ast << std::endl;
			return ast;
		}
		ast::AstPtr visit( ast::IfPtr          ast, util::ScopePtr<value::ValuePtr> globals ) const {
			ast::AstPtr oldTest = ast->test;
			ast::AstPtr newTest = ast->test->visit<ast::AstPtr>( *this, globals );

			ast::AstPtr oldThen = ast->thenBranch;
			ast::AstPtr newThen = ast->thenBranch->visit<ast::AstPtr>( *this, globals );

			ast::AstPtr oldElse = ast->elseBranch;
			ast::AstPtr newElse = ast->elseBranch->visit<ast::AstPtr>( *this, globals );
		
			if ( (oldTest == newTest) && (oldThen == newThen) && (oldElse == newElse) ) return ast;

			return new ast::If( ast->location, newTest, newThen, newElse );
		}
		ast::AstPtr visit( ast::DoPtr          ast, util::ScopePtr<value::ValuePtr> globals ) const {
			DBG( Do );

			std::vector<ast::AstPtr> exprs;
			bool changed = false;

			for ( auto it = ast->begin(), end = ast->end(); it != end; ++it ) {
				ast::AstPtr oldAst = *it;
				ast::AstPtr newAst = oldAst->visit<ast::AstPtr>( *this, globals );

//				if ( oldAst == newAst ) 
//					std::cout << "POOO " << oldAst << std::endl;
//				else
//					std::cout << oldAst << ", NO POO" << std::endl;


				changed = changed || (oldAst != newAst);

				exprs.push_back( newAst );
			}

			if ( changed ) 
				return new ast::Do( ast->location, exprs );
			else {
				return ast;
			}
		}
		ast::AstPtr visit( ast::LetStarPtr     ast, util::ScopePtr<value::ValuePtr> globals ) const {
			DBG( LetStar );

			ast::Let::Bindings bindings;
			bool changed = false;

			for ( auto it = ast->begin(), end = ast->end(); it != end; ++it ) {
				const ast::Let::Binding& b = *it;

				auto name   = b.first;
				auto oldAst = b.second;
				auto newAst = b.second->visit<ast::AstPtr>( *this, globals );

				changed = changed || (oldAst != newAst);

				bindings.push_back( ast::Let::Binding( name, newAst ) );
			}

			auto oldBody = ast->body;
			auto newBody = ast->body->visit<ast::AstPtr>( *this, globals );

			changed = changed || (oldBody!= newBody);

			if ( changed ) 
				return new ast::Let( ast->location, bindings, newBody );
			else
				return ast;
		}
		ast::AstPtr visit( ast::LetPtr         ast, util::ScopePtr<value::ValuePtr> globals ) const {
			DBG( Let );

			ast::Let::Bindings bindings;
			bool changed = false;

			for ( auto it = ast->begin(), end = ast->end(); it != end; ++it ) {
				const ast::Let::Binding& b = *it;

				auto name   = b.first;
				auto oldAst = b.second;
				auto newAst = b.second->visit<ast::AstPtr>( *this, globals );

				changed = changed || (oldAst != newAst);

				bindings.push_back( ast::Let::Binding( name, newAst ) );
			}

			auto oldBody = ast->body;
			auto newBody = ast->body->visit<ast::AstPtr>( *this, globals );

			changed = changed || (oldBody!= newBody);

			if ( changed ) 
				return new ast::Let( ast->location, bindings, newBody );
			else
				return ast;
		}
		ast::AstPtr visit( ast::LambdaPtr      ast, util::ScopePtr<value::ValuePtr> globals ) const {
			DBG( Lambda );

			if ( ast->body ) {
				ast::AstPtr oldBody = ast->body;
				ast::AstPtr newBody = ast->body->visit<ast::AstPtr>( *this, globals );

				if ( oldBody != newBody ) {
					return new ast::Lambda( ast->location, ast->name, ast->params, ast->capture, newBody );
				}
			}
			return ast;
		}
		ast::AstPtr visit( ast::ApplicationPtr ast, util::ScopePtr<value::ValuePtr> globals ) const {
			ast::LambdaPtr lambda = asLambda( ast->fun, globals );
			if ( !lambda ) return ast;
			if ( !(lambda->body) || (lambda->depth() > Jit::inliningThreshold) ) return ast;

			ast::Let::Bindings bindings;
				
			auto name = lambda->params_begin();
			for ( auto it = ast->begin(), end = ast->end(); it != end; ++it, ++name ) {
				auto arg = (*it)->visit<ast::AstPtr>( *this, globals );

				bindings.push_back( ast::Let::Binding( (*name)->name, arg ) );
			}

			ast::LetPtr let = new ast::Let( ast->location, bindings, lambda->body );

			return let;
		}
	};

	auto newFn = dynamic_cast<ast::LambdaPtr>( fn->visit<ast::AstPtr>( Visitor(), globals ) );

	assert( newFn );

//	if ( newFn != fn ) { std::cout << "INLINED " << fn->name << " TO " << newFn << std::endl; }

	return newFn;
}

Jit::SharedData* Jit::shared = nullptr;

Jit::SharedData::SharedData() {
	ctx = jit_context_create();

	tag_t = jit_type_sys_ulong;
	ptr_t = jit_type_sys_ulong;//jit_type_void_ptr;

	jit_type_t* value_fields  = new jit_type_t[1];
	value_fields[0] = tag_t;

	jit_type_t* cons_fields = new jit_type_t[3];
	cons_fields[0] = tag_t;
	cons_fields[1] = ptr_t;
	cons_fields[2] = ptr_t;

	jit_type_t* int_fields    = new jit_type_t[2];
	int_fields[0] = tag_t;
	int_fields[1] = jit_type_long;

	jit_type_t* real_fields   = new jit_type_t[2];
	real_fields[0] = tag_t;
	real_fields[1] = jit_type_float64;

	jit_type_t* string_fields = new jit_type_t[2];
	string_fields[0] = tag_t;
	string_fields[1] = ptr_t;

	jit_type_t* symbol_fields = new jit_type_t[2];
	symbol_fields[0] = tag_t;
	symbol_fields[1] = ptr_t;

	jit_type_t* ref_fields = new jit_type_t[2];
	ref_fields[0] = tag_t;
	ref_fields[0] = ptr_t;

	value_t  = jit_type_create_struct( value_fields,  1, 1 );
	cons_t   = jit_type_create_struct( cons_fields,   3, 1 );
	int_t    = jit_type_create_struct( int_fields,    2, 1 );
	real_t   = jit_type_create_struct( real_fields,   2, 1 );
	string_t = jit_type_create_struct( string_fields, 2, 1 );
	symbol_t = jit_type_create_struct( symbol_fields, 2, 1 );
	ref_t    = jit_type_create_struct( ref_fields,    2, 1 );

	jit_type_t* fail_params = new jit_type_t[1];
	fail_params[0] = ptr_t;

	fail_signature = jit_type_create_signature( jit_abi_cdecl, jit_type_void, fail_params, 1, 1 );
}

jit_type_t Jit::SharedData::SharedData::signature( size_t arity ) {
	auto lb = signature_ts.lower_bound( arity );

	if ( lb != signature_ts.end() && lb->first == arity ) {
		// cache hit
		return lb->second;
	} else {
		// cache miss
		jit_type_t* params = new jit_type_t[arity + 1];

		for ( size_t i = 0; i <= arity; i++ ) {
			params[i] = ptr_t;
		}

		auto ty = jit_type_create_signature( jit_abi_cdecl, ptr_t, params, arity + 1, 1 );
		
		signature_ts.insert( lb, std::make_pair( arity, ty ) );

		return ty;
	}
}


