#ifndef __SCOPE_HPP__
#define __SCOPE_HPP__

#include "lllm/reader/SourceLocation.hpp"
#include "lllm/analyzer/Ast.hpp"

namespace lllm {
	namespace analyzer {
		class LambdaScope;

		class Scope {
			public:
				virtual VariablePtr get( const util::InternedString& name ) = 0;
		};

		class GlobalScope : public Scope {
			public:
				GlobalScope( ScopePtr parent );
				
				VariablePtr get( const util::InternedString& name ) override final;

				void addGlobal( const reader::SourceLocation&, const util::InternedString& name, AstPtr value );
			private:
				std::vector<VariablePtr> values;

				const ScopePtr parent;
		};
		class LambdaScope : public Scope {
			public:
				LambdaScope( ScopePtr parent );

				VariablePtr get( const util::InternedString& name ) override final;

				void addParameter( const reader::SourceLocation&, const util::InternedString& param );

				const std::vector<VariablePtr>& parameters() const;
				const std::vector<VariablePtr>& captured()   const;
			private:
				const ScopePtr parent;

				std::vector<VariablePtr> _parameters;
				std::vector<VariablePtr> _capturedVariables;
		};
		class LocalScope : public Scope {
			public:
				LocalScope( ScopePtr parent );

				VariablePtr get( const util::InternedString& name ) override final;

				void addLocal( const reader::SourceLocation&, const util::InternedString& name, AstPtr value );

				const std::vector<VariablePtr>& bindings() const;
			private:
				const ScopePtr parent;
				
				std::vector<VariablePtr> _bindings;
		};

	}; // end namespace analyzer
}; // end namespace lllm

#endif /* __SCOPE_HPP__ */

