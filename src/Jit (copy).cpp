
#include "lllm/Jit.hpp"
#include "lllm/ast/AstIO.hpp"
#include "lllm/value/ValueIO.hpp"
#include "lllm/util/fail.hpp"
#include "lllm/util/util_io.hpp"

#include <jit/jit.h>
#include <jit/jit-dump.h>

#include <map>
#include <cstdio>
#include <iostream>

#if LLLM_DBG_LVL > 0
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
};
typedef JitScope* JitScopePtr;

class ScopeAdapter final : public JitScope {
	public:
		ScopeAdapter( util::ScopePtr<value::ValuePtr> scope, jit_function_t fn ) : scope( scope ), fn( fn ) {}

		bool lookup( const util::InternedString& name, jit_value_t*   dst ) override final {
			ValuePtr val;
			if ( scope->lookup( name, &val ) ) {
				*dst = jit_value_create_long_constant( fn, jit_type_long, (long)(void*) val );
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
	private:
		const JitScopePtr    parent;
		util::InternedString name;
		jit_function_t       fun;
};


struct Jit::SharedData {
	SharedData();

	jit_context_t ctx;

	jit_type_t value_t;
	jit_type_t cons_t;
	jit_type_t int_t;
	jit_type_t real_t;
	jit_type_t char_t;
	jit_type_t string_t;
	jit_type_t symbol_t;
	jit_type_t ref_t;

	std::map<size_t, jit_type_t> signature_ts;

	jit_type_t signature( size_t arity );
};

static inline int envElementOffset( int elemIdx ) {
	return 24 + (elemIdx * sizeof(ValuePtr));
}

extern "C" {
	static void* lllm_fail_apply( void* v ) {
		LLLM_FAIL( "Cannot apply " << reinterpret_cast<ValuePtr>( v ) );
		return nullptr;
	}
	static void* lllm_alloc_lambda( void* v ) {
		return Lambda::alloc( reinterpret_cast<ast::LambdaPtr>( v ) );
	}
}

void Jit::compile( value::LambdaPtr fn, util::ScopePtr<value::ValuePtr> globals ) {
	if ( !shared ) shared = new SharedData();

	if ( fn->code || fn->data->code ) return; // don't compile twice

	jit_context_build_start( shared->ctx );
	
	ast::LambdaPtr      ast = fn->data->ast;

	jit_function_t fnIr = jit_function_create( shared->ctx, shared->signature( fn->arity() ) );
	
	struct Visitor {
		jit_value_t visit( ast::NilPtr         ast, jit_function_t ir, JitScopePtr scope, bool tail ) const {
			DBG( Nil );
			return jit_value_create_long_constant( ir, jit_type_long, 0 );
		}
		jit_value_t visit( ast::IntPtr         ast, jit_function_t ir, JitScopePtr scope, bool tail ) const {
			DBG( Int );
			return jit_value_create_long_constant( ir, jit_type_long, (long)(void*) number( ast->value ) );
		}
		jit_value_t visit( ast::RealPtr        ast, jit_function_t ir, JitScopePtr scope, bool tail ) const {
			DBG( Real );
			return jit_value_create_long_constant( ir, jit_type_long, (long)(void*) number( ast->value ) );
		}
		jit_value_t visit( ast::CharPtr        ast, jit_function_t ir, JitScopePtr scope, bool tail ) const {
			DBG( Char );
			return jit_value_create_long_constant( ir, jit_type_long, (long)(void*) character( ast->value ) );
		}
		jit_value_t visit( ast::StringPtr      ast, jit_function_t ir, JitScopePtr scope, bool tail ) const {
			DBG( String );
			return jit_value_create_long_constant( ir, jit_type_long, (long)(void*) string( ast->value ) );
		}
		jit_value_t visit( ast::VariablePtr    ast, jit_function_t ir, JitScopePtr scope, bool tail ) const {
			DBG( Variable );
			jit_value_t val;
			if ( scope->lookup( ast->name, &val ) ) {
				return val;	
			} else {
				LLLM_FAIL( ast->location << ": Undefined variable " << ast );
			}
		}
		jit_value_t visit( ast::QuotePtr       ast, jit_function_t ir, JitScopePtr scope, bool tail ) const {
			DBG( Quote );
			return jit_value_create_long_constant( ir, jit_type_long, (long)(void*) ast->value );
		}
		jit_value_t visit( ast::IfPtr          ast, jit_function_t ir, JitScopePtr scope, bool tail ) const {
			DBG( If );

			jit_label_t elseLabel = jit_label_undefined;
			jit_label_t endLabel  = jit_label_undefined;

			jit_value_t result = jit_value_create( ir, jit_type_long );

			// emit test code
			jit_value_t test = ast->test->visit<jit_value_t>( *this, ir, scope, false ); 
			// branch to else part if test == 0
			jit_insn_branch_if_not( ir, test, &elseLabel );    
			// emit then part
			jit_value_t thenResult = ast->thenBranch->visit<jit_value_t>( *this, ir, scope, tail );
			jit_insn_store( ir, result, thenResult );
			jit_insn_branch( ir, &endLabel );
			// emit else part
			jit_insn_label( ir, &elseLabel );
			jit_value_t elseResult = ast->elseBranch->visit<jit_value_t>( *this, ir, scope, tail );
			jit_insn_store( ir, result, elseResult );
			// emit end part
			jit_insn_label( ir, &endLabel );

			return result;
		}
		jit_value_t visit( ast::DoPtr          ast, jit_function_t ir, JitScopePtr scope, bool tail ) const {
			DBG( Do );

			for ( auto it = ast->begin(), end = --(ast->end()); it != end; ++it ) {
				(*it)->visit<jit_value_t>( *this, ir, scope, false );
			}

			return ast->back()->visit<jit_value_t>( *this, ir, scope, tail );
		}
		jit_value_t visit( ast::LetPtr         ast, jit_function_t ir, JitScopePtr scope, bool tail ) const {
			DBG( Let );

			for ( auto it = ast->begin(), end = ast->end(); it != end; ++it ) {
				const ast::Let::Binding& b = *it;

				auto name = b.first;
				auto val  = b.second->visit<jit_value_t>( *this, ir, scope, false );

				val = jit_insn_load( ir, val );

				scope = new JitValueScope( name, val , scope );
			}

			return ast->body->visit<jit_value_t>( *this, ir, scope, tail );
		}
		jit_value_t visit( ast::LambdaPtr      ast, jit_function_t ir, JitScopePtr scope, bool tail ) const {
			DBG( Lambda );

			jit_value_t args[1];
			args[0] = jit_value_create_long_constant( ir, jit_type_long, (long)(void*) ast );

			// create closure
			jit_value_t lambda = jit_insn_call_native( ir, "lllm::value::Lambda::alloc", (void*)lllm_alloc_lambda, 
			                                           shared->signature( 0 ), args, 1, 0 );

			// fill env
			int idx = 0;
			for ( auto it = ast->capturedVariables.begin(), end = ast->capturedVariables.end(); it != end; ++it, ++idx ) {
				jit_value_t cap;
				if ( scope->lookup( (*it)->name, &cap ) ) {
					jit_insn_store_relative( ir, lambda, envElementOffset( idx ), cap );
				} else {
					LLLM_FAIL( (*it)->location << ": Unknown variable " << (*it)->name );
				}
			}

			return lambda;
		}
		jit_value_t visit( ast::ApplicationPtr ast, jit_function_t ir, JitScopePtr scope, bool tail ) const {
			DBG( Application );

			size_t arity = ast->arity();
	
			// emit code for function
			jit_value_t  fun  = ast->fun->visit<jit_value_t>( *this, ir, scope, false );

			// emit code for args
			jit_value_t* args = new jit_value_t[ast->args.size() + 1];
		
			args[0] = fun;

			int idx = 1;
			for ( auto it = ast->begin(), end = ast->end(); it != end; ++it, ++idx ) {
				args[idx] = (*it)->visit<jit_value_t>( *this, ir, scope, false );
			}

			jit_value_t failArgs[1] = { fun };

			// emit type check
			jit_label_t fnIsNull        = jit_label_undefined;
			jit_label_t fnHasWrongArity = jit_label_undefined;
			jit_label_t end             = jit_label_undefined;

			// check for null
			jit_insn_branch_if_not( ir, fun, &fnIsNull );
			// arity check
			jit_value_t typeTag     = jit_insn_load_relative( ir, fun, 0, jit_type_long );
			jit_value_t expectedTag = jit_value_create_long_constant( ir, jit_type_long, size_t(value::Type::Lambda) + arity );
			jit_value_t arityCheck  = jit_insn_eq( ir, typeTag, expectedTag );
			jit_insn_branch_if_not( ir, arityCheck, &fnHasWrongArity );
			// emit call
			jit_value_t code    = jit_insn_load_relative( ir, fun    , 8, jit_type_long );
			jit_value_t result  = jit_insn_call_indirect( ir, code, shared->signature( arity ), args, arity + 1, 0 );
			jit_insn_branch( ir, &end );
			// emit null check fail
			jit_insn_label( ir, &fnIsNull );
			jit_insn_call_native( ir, "lllm_fail_apply", (void*)lllm_fail_apply, shared->signature( 0 ), failArgs, 1, 0 );
			// emit arity check fail
			jit_insn_label( ir, &fnHasWrongArity );
			jit_insn_call_native( ir, "lllm_fail_apply", (void*)lllm_fail_apply, shared->signature( 0 ), failArgs, 1, 0 );			

			// done
			jit_insn_label( ir, &end );
			return result;
		}
		jit_value_t visit( ast::DefinePtr      ast, jit_function_t ir, JitScopePtr scope, bool tail ) const {
			DBG( Define );
			LLLM_FAIL( ast->location << ": Define statements may not appear within a function" );
		}
	};

	JitScopePtr scope = new ScopeAdapter( globals, fnIr );

	int idx;

	// add captured vars to scope
	jit_value_t env = jit_value_get_param( fnIr, 0 );

	idx = 0;
	for ( auto it = ast->capturedVariables.begin(), end = ast->capturedVariables.end(); it != end; ++it, ++idx ) {
		jit_value_t envElem = jit_insn_load_relative( fnIr, env, envElementOffset( idx ), jit_type_long );

		scope = new JitValueScope( (*it)->name, envElem, scope );
	}
	
	// add self to scope for recursion
	if ( std::strcmp( ast->name, "" ) != 0 ) {
		//TODO
		scope = new JitFunctionScope( ast->name, fnIr, scope );
	}

	// add params to scope
	idx = 1;
	for ( auto it = ast->parameters.begin(), end = ast->parameters.end(); it != end; ++it, ++idx ) {
		scope = new JitValueScope( (*it)->name, jit_value_get_param( fnIr, idx ), scope );
	}	

	jit_value_t retVal = ast->body->visit<jit_value_t>( Visitor(), fnIr, scope, true );
//	jit_value_t retVal = jit_value_create_long_constant( fnIr, jit_type_long, 0 );

	jit_insn_return( fnIr, retVal );

//	std::printf("-- ABOUT TO COMPILE %s ------------------------------------\n", (util::CStr)ast->name );
//	jit_dump_function( stderr, fnIr, ast->name );
//	std::printf("-----------------------------------------------------------\n");

	if ( jit_function_compile( fnIr ) == 0 ) {
		LLLM_FAIL( ast->location << "Could not compile function " << ast );
	}

//	std::printf("-- COMPILED %s --------------------------------------------\n", (util::CStr)ast->name );
//	jit_dump_function( stderr, fnIr, ast->name );
//	std::printf("-----------------------------------------------------------\n");

	jit_context_build_end( shared->ctx );

	fn->data->code = (Lambda::FnPtr) jit_function_to_closure( fnIr );

//	printf(">> %p\n", fn->data->code );
}


Jit::SharedData* Jit::shared = nullptr;

Jit::SharedData::SharedData() {
	ctx = jit_context_create();

	jit_type_t* value_fields  = new jit_type_t[1];
	value_fields[0] = jit_type_long;

	jit_type_t* cons_fields = new jit_type_t[3];
	cons_fields[0] = jit_type_long;
	cons_fields[1] = value_t;
	cons_fields[2] = value_t;

	jit_type_t* int_fields    = new jit_type_t[2];
	int_fields[0] = jit_type_long;
	int_fields[1] = jit_type_long;

	jit_type_t* real_fields   = new jit_type_t[2];
	real_fields[0] = jit_type_long;
	real_fields[1] = jit_type_float64;

	jit_type_t* string_fields = new jit_type_t[2];
	string_fields[0] = jit_type_long;
	string_fields[1] = jit_type_long;

	jit_type_t* symbol_fields = new jit_type_t[2];
	symbol_fields[0] = jit_type_long;
	symbol_fields[1] = jit_type_long;

	jit_type_t* ref_fields = new jit_type_t[2];
	ref_fields[0] = jit_type_long;
	ref_fields[0] = jit_type_long;

	value_t  = jit_type_create_struct( value_fields,  1, 1 );
	cons_t   = jit_type_create_struct( cons_fields,   3, 1 );
	int_t    = jit_type_create_struct( int_fields,    2, 1 );
	real_t   = jit_type_create_struct( real_fields,   2, 1 );
	string_t = jit_type_create_struct( string_fields, 2, 1 );
	symbol_t = jit_type_create_struct( symbol_fields, 2, 1 );
	ref_t    = jit_type_create_struct( ref_fields,    2, 1 );
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
			params[i] = jit_type_long;
		}

		auto ty = jit_type_create_signature( jit_abi_cdecl, jit_type_long, params, arity + 1, 1 );
		
		signature_ts.insert( lb, std::make_pair( arity, ty ) );

		return ty;
	}
}

