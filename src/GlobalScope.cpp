
#include "lllm/GlobalScope.hpp"
#include "lllm/Builtins.hpp"
#include "lllm/ast/Ast.hpp"
#include "lllm/value/Value.hpp"

#include <cstring>
#include <iostream>

using namespace lllm;
using namespace lllm::util;

void GlobalScope::add( const util::SourceLocation& loc, const util::InternedString& name, ast::AstPtr ast, value::ValuePtr val ) {
	data[name] = std::make_pair( ast::Variable::makeGlobal( loc, name, ast ), val );
}

bool GlobalScope::lookup( const util::InternedString& name, ast::AstPtr*      dst ) {
	auto lb = data.lower_bound( name );

	if ( lb != data.end() && lb->first == name ) {
		*dst = lb->second.first->ast;
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
bool GlobalScope::lookup( const util::InternedString& name, value::ValuePtr* dst ) {
	auto lb = data.lower_bound( name );

	if ( lb != data.end() && lb->first == name ) {
		*dst = lb->second.second;
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

void GlobalScope::dump() {
	for ( auto it = data.begin(), end = data.end(); it != end; it++ ) {
		std::cout << "*GLO " << it->first << "\t->\t" << it->second.first << "\t->\t" << it->second.second << std::endl;
	}
}
