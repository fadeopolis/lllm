#ifndef __LLLM_BUILTINS_HPP__
#define __LLLM_BUILTINS_HPP__ 1

#include "lllm/lllm.hpp"
#include "lllm/util/Scope.tpp"

#include <map>

namespace lllm {
	class Builtins : public util::Scope<ast::AstPtr>,
	                 public util::Scope<ast::VariablePtr>, 
	                 public util::Scope<value::ValuePtr> {
		public:
			static Builtins& get();

			bool lookup( const util::InternedString& name, ast::AstPtr*      dst ) override final;
			bool lookup( const util::InternedString& name, ast::VariablePtr* dst ) override final;
			bool lookup( const util::InternedString& name, value::ValuePtr*  dst ) override final;
			bool contains( const util::InternedString& name ) override final;

			static const value::ValuePtr CLEAR_MARK;
		private:
			Builtins();

			static Builtins* INSTANCE;
			std::map<util::InternedString,std::pair<ast::VariablePtr,value::ValuePtr>> data;
	};
};

#endif /* __LLLM_BUILTINS_HPP__ */

