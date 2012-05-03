
#include "lllm/analyzer/Scope.hpp"

using namespace lllm;
using namespace lllm::analyzer;

GlobalScope::GlobalScope( ScopePtr parent ) : parent( parent ) {}
LambdaScope::LambdaScope( ScopePtr parent ) : parent( parent ) {}
LocalScope::LocalScope( ScopePtr parent )   : parent( parent ) {}

VariablePtr GlobalScope::get( const util::InternedString& name ) {
	for ( auto it = values.begin(), end = values.end(); it != end; ++it ) {
		VariablePtr var = *it;
		if ( var->name == name ) return var;
	}

	return parent->get( name );
}

VariablePtr LambdaScope::get( const util::InternedString& name ) {
	for ( auto it = _parameters.begin(), end = _parameters.end(); it != end; ++it ) {
		VariablePtr var = *it;
		if ( var->name == name ) return var;
	}

	for ( auto it = _capturedVariables.begin(), end = _capturedVariables.end(); it != end; ++it ) {
		VariablePtr var = *it;
		if ( var->name == name ) return var;
	}

	return parent->get( name );
}
VariablePtr LocalScope::get( const util::InternedString& name ) {
	for ( auto it = _bindings.begin(), end = _bindings.end(); it != end; ++it ) {
		VariablePtr var = *it;
		if ( var->name == name ) return var;
	}

	return parent->get( name );
}

void GlobalScope::addGlobal( const reader::SourceLocation& loc, const util::InternedString& name, AstPtr value ) {
	values.push_back( new Variable( loc, name, value, Variable::GLOBAL ) );
}
void LambdaScope::addParameter( const reader::SourceLocation& loc, const util::InternedString& param ) {
	_parameters.push_back( new Variable( loc, param, nullptr, Variable::PARAMETER ) );
}
void LocalScope::addLocal( const reader::SourceLocation& loc, const util::InternedString& name, AstPtr value ) {
	_bindings.push_back( new Variable( loc, name, value, Variable::LOCAL ) );
}

const std::vector<VariablePtr>& LambdaScope::parameters() const { return _parameters;        }
const std::vector<VariablePtr>& LambdaScope::captured()   const { return _capturedVariables; }
const std::vector<VariablePtr>& LocalScope::bindings()    const { return _bindings;          }

//LambdaScope::iterator LambdaScope::begin() { return parameters.begin(); }
//LambdaScope::iterator LambdaScope::end()   { return parameters.end();   }
//
//LocalScope::iterator LocalScope::begin() { return _bindings.begin(); }
//LocalScope::iterator LocalScope::end()   { return _bindings.end();   }






























