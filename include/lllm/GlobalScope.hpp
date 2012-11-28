#ifndef __LLLM_VM_HPP__
#define __LLLM_VM_HPP__ 1

#include "lllm/util/Scope.tpp"

#include <map>

namespace lllm {
	class GlobalScope : public util::Scope<ast::AstPtr>,
	                    public util::Scope<ast::VariablePtr>,
	                    public util::Scope<value::ValuePtr> {
		public:
			void add( const util::SourceLocation& loc, const util::InternedString& name, ast::AstPtr ast, value::ValuePtr val );

			bool lookup( const util::InternedString& name, ast::AstPtr*      dst ) override final;
			bool lookup( const util::InternedString& name, ast::VariablePtr* dst ) override final;
			bool lookup( const util::InternedString& name, value::ValuePtr*  dst ) override final;

			bool contains( const util::InternedString& name ) override final;

			void dump() override final;
		private:
			std::map<util::InternedString,std::pair<ast::VariablePtr,value::ValuePtr>> data;
	};
};

#endif /* __LLLM_VM_HPP__ */

