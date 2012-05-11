
#include "lllm/util/Env.hpp"

using namespace lllm;
using namespace lllm::util;

Env::Env( const util::InternedString& name, value::ValuePtr val ) : node( new Node( name, val, nullptr ) ) {}
Env::Node::Node( util::InternedString name, value::ValuePtr value, NodePtr next ) :
	name( name ), value( value ), next( next ) {}

Env Env::add( const util::InternedString& name, value::ValuePtr val ) const {
	Env env;

	env.node = new Node( name, val, node );

	return env;
}
bool Env::contains( const util::InternedString& name ) const {
	for ( const Node* node = this->node; node; node = node->next ) {
		if ( node->name == name ) return true;
	}

	return false;
}
bool Env::lookup( const util::InternedString& name, value::ValuePtr* val ) const {
	for ( const Node* node = this->node; node; node = node->next ) {
		if ( node->name == name ) {
			*val = node->value;
			return true;
		}
	}

	return false;
}
