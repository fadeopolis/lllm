#ifndef __PREDEF_HPP__
#define __PREDEF_HPP__ 1

namespace lllm {
	typedef const char* CStr;

	namespace reader {
		class Sexpr;
		typedef const Sexpr* SexprPtr;
		typedef const Sexpr& SexprRef;

		#define LLLM_VISITOR( TYPE ) class TYPE;
		#include "lllm/reader/Sexpr.inc"
		#define LLLM_VISITOR( TYPE ) typedef const TYPE* TYPE##Ptr;
		#include "lllm/reader/Sexpr.inc"

		class SourceLocation;
	};

	namespace analyzer {
		class Ast;
		typedef Ast* AstPtr;
		typedef Ast& AstRef;
		typedef const Ast* ConstAstPtr;
		typedef const Ast& ConstAstRef;

		#define LLLM_VISITOR( TYPE ) class TYPE;
		#include "lllm/analyzer/Ast.inc"
		#define LLLM_VISITOR( TYPE ) typedef TYPE* TYPE##Ptr;
		#include "lllm/analyzer/Ast.inc"
		#define LLLM_VISITOR( TYPE ) typedef TYPE& TYPE##Ref;
		#include "lllm/analyzer/Ast.inc"
		#define LLLM_VISITOR( TYPE ) typedef const TYPE* Const##TYPE##Ptr;
		#include "lllm/analyzer/Ast.inc"
		#define LLLM_VISITOR( TYPE ) typedef const TYPE& Const##TYPE##Ref;
		#include "lllm/analyzer/Ast.inc"
		
		class Scope;
		class GlobalScope;
		class LambdaScope;
		class LocalScope;

		typedef Scope*       ScopePtr;
		typedef GlobalScope* GlobalScopePtr;
		typedef LambdaScope* LambdaScopePtr;
		typedef LocalScope*  LocalScopePtr;
		typedef const Scope*       ConstScopePtr;
		typedef const GlobalScope* ConstGlobalScopePtr;
		typedef const LambdaScope* ConstLambdaScopePtr;
		typedef const LocalScope*  ConstLocalScopePtr;
	};

	namespace value {
		class Value;
		typedef const Value* ValuePtr;
		
		#define LLLM_VISITOR( TYPE ) class TYPE;
		#include "lllm/values/Value.inc"
		#define LLLM_VISITOR( TYPE ) typedef const TYPE* TYPE##Ptr;
		#include "lllm/values/Value.inc"
	}; // end namespace value
	
	namespace eval {
		class Env;
		typedef const Env* EnvPtr;
		typedef const Env& EnvRef;
	};

	namespace jit {

	};
};

#endif /* __PREDEF_HPP__ */

