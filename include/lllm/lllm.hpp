#ifndef __LLLM_HPP__
#define __LLLM_HPP__ 1

struct __float128;

#include <gc_cpp.h>

#define LLLM_DBG_LVL 0

namespace lllm {
	static constexpr long MAX_ARITY = 13;

	class   Obj;
	typedef Obj*       ObjPtr;
	typedef const Obj* ConstObjPtr;

	//***** S-EXPRESSION (PARSE TREE) *****************************************

	namespace sexpr {
		#define LLLM_VISITOR( TYPE )       \
			class TYPE;                    \
			typedef const TYPE* TYPE##Ptr;
		#include "lllm/sexpr/Sexpr.inc"
	};

	//***** ABSTRACT SYNTAX TREE **********************************************

	namespace ast {
		#define LLLM_VISITOR( TYPE )                 \
			class   TYPE;                            \
			typedef TYPE*       TYPE##Ptr;           \
			typedef const TYPE* Const##TYPE##Ptr;
		#include "lllm/ast/Ast.inc"
	};

	//***** RUNTIME VALUES ****************************************************

	namespace value {
		#define LLLM_VISITOR( TYPE )       \
			class TYPE;                    \
			typedef const TYPE* TYPE##Ptr;
		#include "lllm/value/Value.inc"
	};
		
	//***** MAIN INTERFACES ***************************************************

	class   Vm;
	typedef Vm* VmPtr;

	// ** parses strings/files in Sexprs
	class   Reader;
	typedef Reader* ReaderPtr;

	// ** creates ast from sexprs
	class   Analyzer;
	typedef Analyzer* AnalyzerPtr;

	// ** evaluates ast to runtime values
	class   Evaluator;
	typedef Evaluator* EvaluatorPtr;

	// ** emits machine code for funtions
	class   Emitter;
	typedef Emitter* EmitterPtr;

	class   GlobalScope;
	typedef GlobalScope* GlobalScopePtr;

	//***** UTILITIES *********************************************************

	namespace util {
		typedef const char* CStr;

		class InternedString;
		class SourceLocation;

		template<typename T>
		class Scope;
		
		template<typename T>
		using ScopePtr = Scope<T>*;
	};
};

#endif /* __LLLM_HPP__ */
		
