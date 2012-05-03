#ifndef __Ast_HPP__
#define __Ast_HPP__ 1

#include "lllm/predef.hpp"
#include "lllm/reader/Sexpr.hpp"
#include "lllm/reader/SourceLocation.hpp"
#include "lllm/analyzer/Scope.hpp"
#include "lllm/analyzer/TypeSet.hpp"
#include "lllm/util/InternedString.hpp"

#include <vector>

namespace lllm {
	namespace analyzer {
		class Ast {
			private:
				enum class Type {
					#define LLLM_VISITOR( TYPE ) TYPE, 
					#include "lllm/analyzer/Ast.inc"
					// values after Type::Lambda are also lambdas.
					// the arity of the lambda is (type - Type::Lambda).
				};
			public:
				Ast( const Ast::Type& t, const reader::SourceLocation& );

				virtual TypeSet possibleTypes() const = 0;
	
				template<typename Return, typename Visitor, typename... Args>
				Return visit( Visitor&, Args&... );
				template<typename Return, typename Visitor, typename... Args>
				Return visit( Visitor&, Args&... ) const;
				template<typename Return, typename Visitor, typename... Args>
				Return visit( const Visitor&, Args&... );
				template<typename Return, typename Visitor, typename... Args>
				Return visit( const Visitor&, Args&... ) const;

				const reader::SourceLocation location;
			private:
				const Type                   type;

			#define LLLM_VISITOR( TYPE ) friend class TYPE;
			#include "lllm/analyzer/Ast.inc"
		};

		//***** ATOMS                ****************************************************************//
		class Nil final : public Ast {
			public:
				Nil( const reader::SourceLocation& );
	
				TypeSet possibleTypes() const override final;
		};
		class Int final : public Ast {
			public:
				Int( const reader::SourceLocation&, long );

				TypeSet possibleTypes() const override final;
	
				const long value;
		};
		class Real final : public Ast {
			public:
				Real( const reader::SourceLocation&, double );
	
				TypeSet possibleTypes() const override final;
	
				const double value;
		};
		class Char final : public Ast {
			public:
				Char( const reader::SourceLocation&, char );
	
				TypeSet possibleTypes() const override final;

				const char value;
		};
		class String final : public Ast {
			public:
				String( const reader::SourceLocation&, CStr );
	
				TypeSet possibleTypes() const override final;
	
				const CStr value;
		};

		//***** VARIABLES            ****************************************************************//
		class Variable : public Ast {
			public:
				enum StorageType { EXTERN, GLOBAL, CAPTURED, PARAMETER, LOCAL };

				Variable( const reader::SourceLocation&, const util::InternedString&, AstPtr, StorageType );

				TypeSet possibleTypes() const override final;
	
				const util::InternedString name;
				const AstPtr               value;
				const StorageType          storage;

//			friend class GlobalVariable;
//			friend class Parameter;
//			friend class LocalVariable;
//			friend class CapturedVariable;
		};
/*		class GlobalVariable final : public Variable {
			public:
				GlobalVariable( const reader::SourceLocation&, const util::InternedString&, AstPtr value );

				const AstPtr value;
		};
		class Parameter final : public Variable {
			public:
				Parameter( const reader::SourceLocation&, const util::InternedString& );
		};
		class LocalVariable final : public Variable {
			public:
				LocalVariable( const reader::SourceLocation&, const util::InternedString&, AstPtr value );

				const AstPtr value;
		};
		class CapturedVariable final : public Variable {
			public:
				CapturedVariable( const reader::SourceLocation&, const util::InternedString& );
		};
*/
		class Builtin : public Ast {
			public:
				Builtin( const util::InternedString& name, TypeSet possibleTypes );

				TypeSet possibleTypes() const override final;

				const util::InternedString name;
			private:
				const TypeSet              _possibleTypes;
		};

		//***** SPECIAL FORMS        ****************************************************************//
		class Quote : public Ast {
			public:
				Quote( const reader::SourceLocation&, value::ValuePtr value );
	
				TypeSet possibleTypes() const override final;
	
				const value::ValuePtr value;
		};
		class If : public Ast {
			public:
				If( const reader::SourceLocation&, AstPtr test, AstPtr thenBranch, AstPtr elseBranch );
	
				TypeSet possibleTypes() const override final;
	
				const AstPtr test;
				const AstPtr thenBranch;
				const AstPtr elseBranch;
		};
		class Do : public Ast {
			public:
				Do( const reader::SourceLocation&, const std::vector<AstPtr>& exprs );

				TypeSet possibleTypes() const override final;
	
				const std::vector<AstPtr> exprs;
		};
		class Define : public Ast {
			public:
				Define( const reader::SourceLocation&, const util::InternedString& name, AstPtr var );
	
				TypeSet possibleTypes() const override final;
	
				const util::InternedString name;
				const AstPtr               var;
		};
		class Let : public Ast {
			public:
				typedef std::vector<VariablePtr> Bindings;

				Let( const reader::SourceLocation&, const Bindings& bindings, AstPtr expr );

				TypeSet possibleTypes() const override final;

				const Bindings bindings;
				const AstPtr   expr;
		};
		class Lambda : public Ast {
			public:
				typedef std::vector<VariablePtr> VarList;

				Lambda( const reader::SourceLocation&, LambdaPtr parent, const VarList& parameters, const VarList& captured, AstPtr expr );

				TypeSet possibleTypes() const override final;

				size_t arity() const;

				const LambdaPtr parent;
				const VarList   parameters;
				const VarList   capturedVariables;
				const AstPtr    expr;
		};

		//***** FUNCTION APPLICATION ****************************************************************//
		class Application : public Ast {
			public:
				Application( const reader::SourceLocation&, AstPtr fun, const std::vector<AstPtr>& args );
	
				TypeSet possibleTypes() const override final;
	
				const AstPtr              fun;
				const std::vector<AstPtr> args;
		};

		//***** VISITORS             ****************************************************************//
		template<typename Return, typename Visitor, typename... Args>
		Return Ast::visit( Visitor& v, Args&... args ) {
			switch ( type ) {
				#define LLLM_VISITOR( TYPE ) case Ast::Type::TYPE: return v.visit( dynamic_cast<TYPE##Ptr>( this ), args... );
				#include "lllm/analyzer/Ast.inc"
				default: return v.visit( dynamic_cast<LambdaPtr>( this ), args... );
			}
		}
		template<typename Return, typename Visitor, typename... Args>
		Return Ast::visit( Visitor& v, Args&... args ) const {
			switch ( type ) {
				#define LLLM_VISITOR( TYPE ) case Ast::Type::TYPE: return v.visit( dynamic_cast<Const##TYPE##Ptr>( this ), args... );
				#include "lllm/analyzer/Ast.inc"
				default: return v.visit( dynamic_cast<ConstLambdaPtr>( this ), args... );
			}
		}
		template<typename Return, typename Visitor, typename... Args>
		Return Ast::visit( const Visitor& v, Args&... args ) {
			switch ( type ) {
				#define LLLM_VISITOR( TYPE ) case Ast::Type::TYPE: return v.visit( dynamic_cast<TYPE##Ptr>( this ), args... );
				#include "lllm/analyzer/Ast.inc"
				default: return v.visit( dynamic_cast<LambdaPtr>( this ), args... );
			}
		}
		template<typename Return, typename Visitor, typename... Args>
		Return Ast::visit( const Visitor& v, Args&... args ) const {
			switch ( type ) {
				#define LLLM_VISITOR( TYPE ) case Ast::Type::TYPE: return v.visit( dynamic_cast<Const##TYPE##Ptr>( this ), args... );
				#include "lllm/analyzer/Ast.inc"
				default: return v.visit( dynamic_cast<ConstLambdaPtr>( this ), args... );
			}
		}

	}; // end namespace analyzer
}; // end namespace lllm

#endif /* __Ast_HPP__ */

