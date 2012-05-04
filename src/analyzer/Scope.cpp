
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

GlobalPtr    GlobalScope::addGlobal( const reader::SourceLocation& loc, const util::InternedString& name, AstPtr value ) {
	auto var = new Global( loc, name, value );
	values.push_back( var );
	return var;
}
ParameterPtr LambdaScope::addParameter( const reader::SourceLocation& loc, const util::InternedString& name ) {
	auto var = new Parameter( loc, name );
	_parameters.push_back( var );
	return var;
}
LocalPtr     LocalScope::addLocal( const reader::SourceLocation& loc, const util::InternedString& name, AstPtr value ) {
	auto var = new Local( loc, name, value );
	_bindings.push_back( var );
	return var;
}

const std::vector<ParameterPtr>& LambdaScope::parameters() const { return _parameters;        }
const std::vector<CapturedPtr>&  LambdaScope::captured()   const { return _capturedVariables; }
const std::vector<LocalPtr>&     LocalScope::bindings()    const { return _bindings;          }

//LambdaScope::iterator LambdaScope::begin() { return parameters.begin(); }
//LambdaScope::iterator LambdaScope::end()   { return parameters.end();   }
//
//LocalScope::iterator LocalScope::begin() { return _bindings.begin(); }
//LocalScope::iterator LocalScope::end()   { return _bindings.end();   }






























