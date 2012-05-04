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
					#include "lllm/analyzer/Ast_concrete_only.inc"
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
			private:
				Variable( Type, const reader::SourceLocation&, const util::InternedString& );
			public:
				const util::InternedString name;

			friend class Builtin;
			friend class Global;
			friend class Parameter;
			friend class Captured;
			friend class Local;
		};
		class Builtin final : public Variable {
			public:
				Builtin( const util::InternedString&, TypeSet );

				TypeSet possibleTypes() const override final;
			private:
				const TypeSet _types;
		};
		class Global final : public Variable {
			public:
				Global( const reader::SourceLocation&, const util::InternedString&, AstPtr value );

				TypeSet possibleTypes() const override final;

				const AstPtr value;
		};
		class Captured final : public Variable {
			public:
				Captured( const reader::SourceLocation&, const util::InternedString&, TypeSet );

				TypeSet possibleTypes() const override final;
			private:
				const TypeSet _types;
		};
		class Parameter final : public Variable {
			public:
				Parameter( const reader::SourceLocation&, const util::InternedString& );

				TypeSet possibleTypes() const override final;
		};
		class Local final : public Variable {
			public:
				Local( const reader::SourceLocation&, const util::InternedString&, AstPtr value );

				TypeSet possibleTypes() const override final;

				const AstPtr value;
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
		class Let : public Ast {
			public:
				Let( const reader::SourceLocation&, const std::vector<LocalPtr>&, AstPtr );

				TypeSet possibleTypes() const override final;

				const std::vector<LocalPtr> bindings;
				const AstPtr                body;
		};
		class Lambda : public Ast {
			public:
				Lambda( const reader::SourceLocation&, LambdaPtr parent, 
				        const std::vector<ParameterPtr>&, 
				        const std::vector<CapturedPtr>&, 
				        AstPtr );

				TypeSet possibleTypes() const override final;

				size_t arity() const;

				const LambdaPtr                 parent;
				const std::vector<ParameterPtr> parameters;
				const std::vector<CapturedPtr>  capturedVariables;
				const AstPtr                    body;
		};
		class Define : public Ast {
			public:
				Define( const reader::SourceLocation&, GlobalPtr var );
	
				TypeSet possibleTypes() const override final;
	
				const GlobalPtr var;
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
				#include "lllm/analyzer/Ast_concrete_only.inc"
				default: return v.visit( dynamic_cast<LambdaPtr>( this ), args... );
			}
		}
		template<typename Return, typename Visitor, typename... Args>
		Return Ast::visit( Visitor& v, Args&... args ) const {
			switch ( type ) {
				#define LLLM_VISITOR( TYPE ) case Ast::Type::TYPE: return v.visit( dynamic_cast<Const##TYPE##Ptr>( this ), args... );
				#include "lllm/analyzer/Ast_concrete_only.inc"
				default: return v.visit( dynamic_cast<ConstLambdaPtr>( this ), args... );
			}
		}
		template<typename Return, typename Visitor, typename... Args>
		Return Ast::visit( const Visitor& v, Args&... args ) {
			switch ( type ) {
				#define LLLM_VISITOR( TYPE ) case Ast::Type::TYPE: return v.visit( dynamic_cast<TYPE##Ptr>( this ), args... );
				#include "lllm/analyzer/Ast_concrete_only.inc"
				default: return v.visit( dynamic_cast<LambdaPtr>( this ), args... );
			}
		}
		template<typename Return, typename Visitor, typename... Args>
		Return Ast::visit( const Visitor& v, Args&... args ) const {
			switch ( type ) {
				#define LLLM_VISITOR( TYPE ) case Ast::Type::TYPE: return v.visit( dynamic_cast<Const##TYPE##Ptr>( this ), args... );
				#include "lllm/analyzer/Ast_concrete_only.inc"
				default: return v.visit( dynamic_cast<ConstLambdaPtr>( this ), args... );
			}
		}

	}; // end namespace analyzer
}; // end namespace lllm

#endif /* __Ast_HPP__ */

