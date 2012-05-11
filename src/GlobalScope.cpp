
#include "lllm/GlobalScope.hpp"
#include "lllm/Builtins.hpp"
#include "lllm/ast/Ast.hpp"
#include "lllm/value/Value.hpp"

#include <cstring>

using namespace lllm;
using namespace lllm::util;

void GlobalScope::add( const util::SourceLocation& loc, const util::InternedString& name, value::ValuePtr val ) {
	data[name] = std::make_pair( new ast::Variable( loc, name, value::typeOf( val ), true ), val );
}

bool GlobalScope::lookup( const util::InternedString& name, value::ValuePtr* dst ) {
	auto lb = data.lower_bound( name );

	if ( lb != data.end() && lb->first == name ) {
		*dst = lb->second.second;
		return true;
	} else {
		return Builtins::get().lookup( name, dst );
	}
}
bool GlobalScope::lookup( const util::InternedString& name, ast::VariablePtr* dst ) {
	auto lb = data.lower_bound( name );

	if ( lb != data.end() && lb->first == name ) {
		*dst = lb->second.first;
		return true;
	} else {
		return Builtins::get().lookup( name, dst );
	}
}

bool GlobalScope::contains( const util::InternedString& name ) {
	auto lb = data.lower_bound( name );

	if ( lb != data.end() && lb->first == name ) {
		return true;
	} else {
		return Builtins::get().contains( name );
	}
}

