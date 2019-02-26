#ifndef __Ast_HPP__
#define __Ast_HPP__ 1

#include "lllm/lllm.hpp"
#include "lllm/Obj.hpp"
#include "lllm/sexpr/Sexpr.hpp"
#include "lllm/util/InternedString.hpp"
#include "lllm/util/SourceLocation.hpp"
#include "lllm/util/TypeSet.hpp"
#include "lllm/util/EscapeStatus.hpp"

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
				virtual size_t        depth()         const = 0;
	
				template<typename T>
				T* as();

				template<typename Return, typename Visitor, typename... Args>
				Return visit( Visitor&&, Args... );
				template<typename Return, typename Visitor, typename... Args>
				Return visit( Visitor&&, Args... ) const;
				// template<typename Return, typename Visitor, typename... Args>
				// Return visit( const Visitor&, Args... );
				// template<typename Return, typename Visitor, typename... Args>
				// Return visit( const Visitor&, Args... ) const;

				const util::SourceLocation location;
			private:
				const Type                   type;

			#define LLLM_VISITOR( TYPE ) friend class TYPE;
			#include "lllm/ast/Ast.inc"
		};

		//***** ATOMS                ****************************************************************//
		class Atom : public Ast {
			public:
				util::EscapeStatus escape;
			private:
				Atom( Type type, const util::SourceLocation& );

				size_t        depth()         const override final;

			friend class Nil;
			friend class Int;
			friend class Real;
			friend class Char;
			friend class String;
		};
		class Nil final : public Atom {
			public:
				Nil( const util::SourceLocation& );
	
				util::TypeSet possibleTypes() const override final;
		};
		class Int final : public Atom {
			public:
				Int( const util::SourceLocation&, long );

				util::TypeSet possibleTypes() const override final;
	
				const long value;
		};
		class Real final : public Atom {
			public:
				Real( const util::SourceLocation&, double );
	
				util::TypeSet possibleTypes() const override final;
	
				const double value;
		};
		class Char final : public Atom {
			public:
				Char( const util::SourceLocation&, char );
	
				util::TypeSet possibleTypes() const override final;

				const char value;
		};
		class String final : public Atom {
			public:
				String( const util::SourceLocation&, util::CStr );
	
				util::TypeSet possibleTypes() const override final;

				const util::CStr value;
		};

		//***** VARIABLES            ****************************************************************//
		class Variable : public Ast {
			public:
				static VariablePtr makeGlobal( const util::SourceLocation&, const util::InternedString&, const AstPtr ast );
				static VariablePtr makeLocal( const util::SourceLocation&, const util::InternedString&, const AstPtr ast );
				static VariablePtr makeParameter( const util::SourceLocation&, const util::InternedString& );
				static VariablePtr makeCaptured( const util::SourceLocation&, const util::InternedString&, const AstPtr ast );

				util::TypeSet possibleTypes() const override final;
				size_t        depth()         const override final;

				// info collected at construction time
				const util::InternedString name;
				const AstPtr               ast; // null for parameters!
				const bool                 hasGlobalStorage;

				// info collected later
				bool         getsCaptured;
			private:
				Variable( const util::SourceLocation&, const util::InternedString&, const AstPtr ast, bool global = false );
		};

		//***** SPECIAL FORMS        ****************************************************************//
		class Quote : public Ast {
			public:
				Quote( const util::SourceLocation&, value::ValuePtr value );
	
				util::TypeSet possibleTypes() const override final;
				size_t        depth()         const override final;
	
				const value::ValuePtr value;
		};
		class If : public Ast {
			public:
				If( const util::SourceLocation&, AstPtr test, AstPtr thenBranch, AstPtr elseBranch );
	
				util::TypeSet possibleTypes() const override final;
				size_t        depth()         const override final;
	
				const AstPtr test;
				const AstPtr thenBranch;
				const AstPtr elseBranch;
			private:
				size_t _depth;
		};
		class Do : public Ast {
			public:
				Do( const util::SourceLocation&, const std::vector<AstPtr>& exprs );

				util::TypeSet possibleTypes() const override final;
				size_t        depth()         const override final;
	
				std::vector<AstPtr>::const_iterator begin() const;
				std::vector<AstPtr>::const_iterator end()   const;
				
				AstPtr back() const;

				const std::vector<AstPtr> exprs;
			private:
				size_t _depth;
		};
		class Let : public Ast {
			public:
				typedef std::pair<util::InternedString,AstPtr> Binding;
				typedef std::vector<Binding>                   Bindings;

				Let( const util::SourceLocation&, const Bindings&, AstPtr );

				util::TypeSet possibleTypes() const override final;
				size_t        depth()         const override final;

				Bindings::const_iterator begin() const;
				Bindings::const_iterator end()   const;

				const Bindings bindings;
				const AstPtr   body;
			private:
				size_t _depth;
		};
		class LetStar : public Ast {
			public:
				typedef std::pair<util::InternedString,AstPtr> Binding;
				typedef std::vector<Binding>                   Bindings;

				LetStar( const util::SourceLocation&, const Bindings&, AstPtr );

				util::TypeSet possibleTypes() const override final;
				size_t        depth()         const override final;

				Bindings::const_iterator begin() const;
				Bindings::const_iterator end()   const;

				const Bindings bindings;
				const AstPtr   body;
			private:
				size_t _depth;
		};
		class Lambda : public Ast {
			public:
				typedef VariablePtr              Binding;
				typedef std::vector<Binding>     Bindings;
				typedef Bindings::const_iterator Iterator;

				Lambda( const util::SourceLocation&, 
				        const util::InternedString& name,
						const Bindings& params,
						const Bindings& capture,
				        AstPtr body );

				util::TypeSet possibleTypes() const override final;
				size_t        depth()         const override final;

				size_t arity()   const;
				size_t envSize() const;

				Iterator params_begin() const;
				Iterator params_end()   const;

				Iterator capture_begin() const;
				Iterator capture_end()   const;

				util::EscapeStatus paramEscape( Iterator param );
				void               paramEscape( Iterator param, util::EscapeStatus );

				const util::InternedString   name;
				const AstPtr                 body;
				const Bindings               params;
				const Bindings               capture;
				const value::Lambda::DataPtr data;
			private:
				std::vector<util::EscapeStatus> escapes;
		};
		class Define : public Ast {
			public:
				Define( const util::SourceLocation&, const util::InternedString& name, AstPtr ast );
	
				util::TypeSet possibleTypes() const override final;
				size_t        depth()         const override final;
	
				const util::InternedString name;
				const AstPtr               expr;
		};

		//***** FUNCTION APPLICATION ****************************************************************//
		class Application : public Ast {
			public:
				typedef std::vector<AstPtr>::const_iterator iterator;

				Application( const util::SourceLocation&, AstPtr fun, const std::vector<AstPtr>& args );
	
				util::TypeSet possibleTypes() const override final;
				size_t        depth()         const override final;
	
				const AstPtr              fun;
				const std::vector<AstPtr> args;

				iterator begin() const;
				iterator end()   const;
				size_t   arity() const;
			private:
				size_t _depth;
		};

		//***** VISITORS             ****************************************************************//
		template<typename T>
		T* Ast::as() { return dynamic_cast<T*>( this ); }

		template<typename Return, typename Visitor, typename... Args>
		Return Ast::visit( Visitor&& v, Args... args ) {
			AST_VISIT_DBG_BEGIN;
			switch ( type ) {
				#define LLLM_VISITOR( TYPE ) case ast::Type::TYPE: return v.visit( dynamic_cast<TYPE##Ptr>( this ), args... );
				#include "lllm/ast/Ast_concrete.inc"
				default: return v.visit( dynamic_cast<LambdaPtr>( this ), args... );
			}
			AST_VISIT_DBG_END;
		}
		template<typename Return, typename Visitor, typename... Args>
		Return Ast::visit( Visitor&& v, Args... args ) const {
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

