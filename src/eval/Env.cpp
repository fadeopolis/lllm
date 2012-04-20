
#include "Env.hpp"

#include <cstring>

namespace lllm {
	struct HeadEnv : public Env {
		HeadEnv( const char* key, ValuePtr val, EnvPtr env );
			
		EnvPtr store( const char* key, ValuePtr val )   const final override;
		bool   contains( const char* key )              const final override;
		bool   lookup( const char* key, ValuePtr* dst ) const final override;

		const char* key;
		ValuePtr    val;
		EnvPtr      env;
	};
	struct TailEnv : public Env {
		EnvPtr store( const char* key, ValuePtr val )   const final override;
		bool   contains( const char* key )              const final override;
		bool   lookup( const char* key, ValuePtr* dst ) const final override;
	};

	HeadEnv::HeadEnv( const char* key, ValuePtr val, EnvPtr env ) : key( key ), val( val ), env( env ) {}
	
	EnvPtr HeadEnv::store( const char* key, ValuePtr val ) const {
		return new HeadEnv( key, val, this );
	}
	bool HeadEnv::contains( const char* key ) const {
		if ( std::strcmp( key, this->key ) == 0 ) {
			return true;
		} else {
			return env->contains( key );
		}
	}
	bool HeadEnv::lookup( const char* key, ValuePtr* dst ) const {
		if ( std::strcmp( key, this->key ) == 0 ) {
			*dst = val;
			return true;
		} else {
			return env->lookup( key, dst );
		}
	}

	EnvPtr TailEnv::store( const char* key, ValuePtr val )   const { return new HeadEnv( key, val, this ); }
	bool   TailEnv::contains( const char* key )              const { return false; }
	bool   TailEnv::lookup( const char* key, ValuePtr* dst ) const { return false; }

	EnvPtr Env::make() { return new TailEnv(); }
}


