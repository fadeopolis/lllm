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

				GlobalPtr addGlobal( const reader::SourceLocation&, const util::InternedString& name, AstPtr value );
			private:
				std::vector<GlobalPtr> values;

				const ScopePtr parent;
		};
		class LambdaScope : public Scope {
			public:
				LambdaScope( ScopePtr parent );

				VariablePtr get( const util::InternedString& name ) override final;

				ParameterPtr addParameter( const reader::SourceLocation&, const util::InternedString& param );

				const std::vector<ParameterPtr>& parameters() const;
				const std::vector<CapturedPtr>&  captured()   const;
			private:
				const ScopePtr parent;

				std::vector<ParameterPtr> _parameters;
				std::vector<CapturedPtr>  _capturedVariables;
		};
		class LocalScope : public Scope {
			public:
				LocalScope( ScopePtr parent );

				VariablePtr get( const util::InternedString& name ) override final;

				LocalPtr addLocal( const reader::SourceLocation&, const util::InternedString& name, AstPtr value );

				const std::vector<LocalPtr>& bindings() const;
			private:
				const ScopePtr parent;
				
				std::vector<LocalPtr> _bindings;
		};

	}; // end namespace analyzer
}; // end namespace lllm

#endif /* __SCOPE_HPP__ */

