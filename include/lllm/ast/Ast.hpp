#ifndef __Ast_HPP__
#define __Ast_HPP__ 1

#include "lllm/lllm.hpp"
#include "lllm/Obj.hpp"
#include "lllm/sexpr/Sexpr.hpp"
#include "lllm/util/InternedString.hpp"
#include "lllm/util/SourceLocation.hpp"
#include "lllm/util/TypeSet.hpp"

#include <vector>

#if LLLM_DBG_LVL >= 4
#	include <iostream>
#	include <iomanip>
#	define AST_VISIT_DBG_BEGIN ((void) ({ std::cout << "{VISITING " << (void*)this << " Of TYPE " << (size_t)type << "}" << std::flush; nullptr; }))
#	define AST_VISIT_DBG_END   ((void) ({ std::cout << "{VISITED  " << (void*)this << " OF TYPE " << (size_t)type << "}" << std::flush; nullptr; }))
#else
#	define AST_VISIT_DBG_BEGIN
#	define AST_VISIT_DBG_END
#endif

namespace lllm {
	namespace ast {
		enum class Type {
			#define LLLM_VISITOR( TYPE ) TYPE, 
			#include "lllm/ast/Ast_concrete.inc"
		};

		class Ast : public Obj {
			private:
				Ast( Type type, const util::SourceLocation& loc );
			public:
				virtual util::TypeSet possibleTypes() const = 0;
	
				template<typename T>
				T* as();

				template<typename Return, typename Visitor, typename... Args>
				Return visit( Visitor&, Args... );
				template<typename Return, typename Visitor, typename... Args>
				Return visit( Visitor&, Args... ) const;
				template<typename Return, typename Visitor, typename... Args>
				Return visit( const Visitor&, Args... );
				template<typename Return, typename Visitor, typename... Args>
				Return visit( const Visitor&, Args... ) const;

				const util::SourceLocation location;
			private:
				const Type                   type;

			#define LLLM_VISITOR( TYPE ) friend class TYPE;
			#include "lllm/ast/Ast.inc"
		};

		//***** ATOMS                ****************************************************************//
		class Nil final : public Ast {
			public:
				Nil( const util::SourceLocation& );
	
				util::TypeSet possibleTypes() const override final;
		};
		class Int final : public Ast {
			public:
				Int( const util::SourceLocation&, long );

				util::TypeSet possibleTypes() const override final;
	
				const long value;
		};
		class Real final : public Ast {
			public:
				Real( const util::SourceLocation&, double );
	
				util::TypeSet possibleTypes() const override final;
	
				const double value;
		};
		class Char final : public Ast {
			public:
				Char( const util::SourceLocation&, char );
	
				util::TypeSet possibleTypes() const override final;

				const char value;
		};
		class String final : public Ast {
			public:
				String( const util::SourceLocation&, util::CStr );
	
				util::TypeSet possibleTypes() const override final;
	
				const util::CStr value;
		};

		//***** VARIABLES            ****************************************************************//
		class Variable : public Ast {
			public:
				Variable( const util::SourceLocation&, const util::InternedString&, const util::TypeSet& types, bool global = false );

				util::TypeSet possibleTypes() const override final;

				// info collected at construction time
				const util::InternedString name;
				const bool                 hasGlobalStorage;

				// info collected later
				bool          getsCaptured;
				util::TypeSet types;

			friend class Builtin;
			friend class Global;
			friend class Parameter;
			friend class Captured;
			friend class Local;
		};
/*
		class Builtin final : public Variable {
			public:
				Builtin( const util::InternedString&, util::TypeSet );

				util::TypeSet possibleTypes()    const override final;
				bool          hasGlobalStorage() const override final;
			private:
				const util::TypeSet _types;
		};
		class Global final : public Variable {
			public:
				Global( const util::SourceLocation&, const util::InternedString&, AstPtr value );

				util::TypeSet possibleTypes()    const override final;
				bool          hasGlobalStorage() const override final;

				const AstPtr value;
		};
		class Captured final : public Variable {
			public:
				Captured( const util::SourceLocation&, const util::InternedString&, util::TypeSet );

				util::TypeSet possibleTypes()    const override final;
				bool          hasGlobalStorage() const override final;
			private:
				const util::TypeSet _types;
		};
		class Parameter final : public Variable {
			public:
				Parameter( const util::SourceLocation&, const util::InternedString& );

				util::TypeSet possibleTypes()    const override final;
				bool          hasGlobalStorage() const override final;
		};
		class Local final : public Variable {
			public:
				Local( const util::SourceLocation&, const util::InternedString&, AstPtr value );

				util::TypeSet possibleTypes()    const override final;
				bool          hasGlobalStorage() const override final;

				const AstPtr value;
		};
*/
		//***** SPECIAL FORMS        ****************************************************************//
		class Quote : public Ast {
			public:
				Quote( const util::SourceLocation&, value::ValuePtr value );
	
				util::TypeSet possibleTypes() const override final;
	
				const value::ValuePtr value;
		};
		class If : public Ast {
			public:
				If( const util::SourceLocation&, AstPtr test, AstPtr thenBranch, AstPtr elseBranch );
	
				util::TypeSet possibleTypes() const override final;
	
				const AstPtr test;
				const AstPtr thenBranch;
				const AstPtr elseBranch;
		};
		class Do : public Ast {
			public:
				Do( const util::SourceLocation&, const std::vector<AstPtr>& exprs );

				util::TypeSet possibleTypes() const override final;
	
				std::vector<AstPtr>::const_iterator begin() const;
				std::vector<AstPtr>::const_iterator end()   const;
				
				AstPtr back() const;

				const std::vector<AstPtr> exprs;
		};
		class Let : public Ast {
			public:
				typedef std::pair<util::InternedString,AstPtr> Binding;
				typedef std::vector<Binding>                   Bindings;

				Let( const util::SourceLocation&, const Bindings&, AstPtr );

				util::TypeSet possibleTypes() const override final;

				Bindings::const_iterator begin() const;
				Bindings::const_iterator end()   const;

				const Bindings bindings;
				const AstPtr   body;
		};
		class Lambda : public Ast {
			public:
				typedef VariablePtr          Binding;
				typedef std::vector<Binding> Bindings;

				Lambda( const util::SourceLocation&, 
				        LambdaPtr parent, 
				        const util::InternedString& name,
				        const Bindings&, 
				        const Bindings&, 
				        AstPtr );

				util::TypeSet possibleTypes() const override final;

				size_t arity()   const;
				size_t envSize() const;

				const LambdaPtr            parent;
				const util::InternedString name;
				value::Lambda::Data* const data;
				const Bindings             parameters;
				const Bindings             capturedVariables;
				const AstPtr               body;
		};
		class Define : public Ast {
			public:
				Define( const util::SourceLocation&, const util::InternedString& name, AstPtr ast );
	
				util::TypeSet possibleTypes() const override final;
	
				const util::InternedString name;
				const AstPtr               expr;
		};

		//***** FUNCTION APPLICATION ****************************************************************//
		class Application : public Ast {
			public:
				typedef std::vector<AstPtr>::const_iterator iterator;

				Application( const util::SourceLocation&, AstPtr fun, const std::vector<AstPtr>& args );
	
				util::TypeSet possibleTypes() const override final;
	
				const AstPtr              fun;
				const std::vector<AstPtr> args;

				iterator begin() const;
				iterator end()   const;
				size_t   arity() const;
		};

		//***** VISITORS             ****************************************************************//
		template<typename T>
		T* Ast::as() { return dynamic_cast<T*>( this ); }

		template<typename Return, typename Visitor, typename... Args>
		Return Ast::visit( Visitor& v, Args... args ) {
			AST_VISIT_DBG_BEGIN;
			switch ( type ) {
				#define LLLM_VISITOR( TYPE ) case ast::Type::TYPE: return v.visit( dynamic_cast<TYPE##Ptr>( this ), args... );
				#include "lllm/ast/Ast_concrete.inc"
				default: return v.visit( dynamic_cast<LambdaPtr>( this ), args... );
			}
			AST_VISIT_DBG_END;
		}
		template<typename Return, typename Visitor, typename... Args>
		Return Ast::visit( Visitor& v, Args... args ) const {
			AST_VISIT_DBG_BEGIN;
			switch ( type ) {
				#define LLLM_VISITOR( TYPE ) case ast::Type::TYPE: return v.visit( dynamic_cast<Const##TYPE##Ptr>( this ), args... );
				#include "lllm/ast/Ast_concrete.inc"
				default: return v.visit( dynamic_cast<ConstLambdaPtr>( this ), args... );
			}
			AST_VISIT_DBG_END;
		}
		template<typename Return, typename Visitor, typename... Args>
		Return Ast::visit( const Visitor& v, Args... args ) {
			AST_VISIT_DBG_BEGIN;
			switch ( type ) {
				#define LLLM_VISITOR( TYPE ) case ast::Type::TYPE: return v.visit( dynamic_cast<TYPE##Ptr>( this ), args... );
				#include "lllm/ast/Ast_concrete.inc"
				default: return v.visit( dynamic_cast<LambdaPtr>( this ), args... );
			}
			AST_VISIT_DBG_END;
		}
		template<typename Return, typename Visitor, typename... Args>
		Return Ast::visit( const Visitor& v, Args... args ) const {
			AST_VISIT_DBG_BEGIN;
			switch ( type ) {
				#define LLLM_VISITOR( TYPE ) case ast::Type::TYPE: return v.visit( dynamic_cast<Const##TYPE##Ptr>( this ), args... );
				#include "lllm/ast/Ast_concrete.inc"
				default: return v.visit( dynamic_cast<ConstLambdaPtr>( this ), args... );
			}
			AST_VISIT_DBG_END;
		}

		template<typename... T>
		ApplicationPtr apply( AstPtr fn, std::initializer_list<T...> args ) {
			return new Application( util::SourceLocation("*api*"), fn, std::vector<AstPtr>( args ) );
		}
	};
};

#endif /* __Ast_HPP__ */

