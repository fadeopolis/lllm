
#include "lllm/eval/Env.hpp"

#include <cstring>

using namespace lllm;
using namespace lllm::eval;
using namespace lllm::value;

EnvPtr Env::make( CStr key, value::ValuePtr val ) {
	return new Env( nullptr, key, val );
}

Env::Env( EnvPtr parent, CStr key, ValuePtr val ) : 
	parent( parent ),
	binding( key, val ) {}

bool   Env::contains( CStr key ) const {
	if ( binding.key && std::strcmp( binding.key, key ) == 0 ) {
		return true;
	} else if ( EnvPtr env = this->parent ) {
		return env->contains( key );
	} else {
		return false;
	}
}
bool Env::lookup( CStr key, ValuePtr* val ) const {
	if ( binding.key && std::strcmp( binding.key, key ) == 0 ) {
		*val = binding.val;
		return true;
	} else if ( EnvPtr env = this->parent ) {
		return env->lookup( key, val );
	} else {
		return false;
	}
}
EnvPtr Env::put( CStr key, ValuePtr val ) const {
	return new Env( this, key, val );
}
EnvPtr Env::put( const Binding& b ) const {
	return new Env( this, b.key, b.val );
}


