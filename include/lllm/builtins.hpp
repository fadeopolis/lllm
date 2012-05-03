#ifndef __BUILTINS_HPP__
#define __BUILTINS_HPP__

#include "lllm/analyzer/Scope.hpp"

namespace lllm {
	class BuiltinScope;

	typedef BuiltinScope* BuiltinScopePtr;

	class BuiltinScope : public analyzer::Scope {
		public:
			static BuiltinScope builtins;

			analyzer::VariablePtr get( const util::InternedString& name ) override final;
		
			eval::EnvPtr env() const;
		private:
			BuiltinScope();
			BuiltinScope& operator=( const BuiltinScope& ) = delete;

			void add( const util::InternedString& name, value::ValuePtr val, analyzer::VariablePtr ast );
	};
}; // end namespace lllm

#endif /* __BUILTINS_HPP__ */

