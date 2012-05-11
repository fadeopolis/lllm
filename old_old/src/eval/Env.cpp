
#include "Env.hpp"

#include <cstring>

namespace lllm {
	struct HeadEnv : public Env {
		HeadEnv( const char* key, ValuePtr val, EnvPtr env );
			
		EnvPtr merge( EnvPtr env )                      const final override;
		EnvPtr store( const char* key, ValuePtr val )   const final override;
		bool   contains( const char* key )              const final override;
		bool   lookup( const char* key, ValuePtr* dst ) const final override;

		const char* key;
		ValuePtr    val;
		EnvPtr      env;
	};
	struct MergeEnv : public Env {
		MergeEnv( EnvPtr a, EnvPtr b );

		EnvPtr merge( EnvPtr env )                      const final override;
		EnvPtr store( const char* key, ValuePtr val )   const final override;
		bool   contains( const char* key )              const final override;
		bool   lookup( const char* key, ValuePtr* dst ) const final override;

		const EnvPtr a, b;
	};
	struct TailEnv : public Env {
		EnvPtr merge( EnvPtr env )                      const final override;
		EnvPtr store( const char* key, ValuePtr val )   const final override;
		bool   contains( const char* key )              const final override;
		bool   lookup( const char* key, ValuePtr* dst ) const final override;
	};

	HeadEnv::HeadEnv( const char* key, ValuePtr val, EnvPtr env ) : key( key ), val( val ), env( env ) {}
	MergeEnv::MergeEnv( EnvPtr a, EnvPtr b ) : a( a ), b( b ) {}	

	EnvPtr HeadEnv::merge( EnvPtr env ) const {
		return new MergeEnv( this, env );
	}
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

	EnvPtr MergeEnv::merge( EnvPtr env ) const {
		return new MergeEnv( this, env );
	}
	EnvPtr MergeEnv::store( const char* key, ValuePtr val ) const {
		return new HeadEnv( key, val, this );
	}
	bool MergeEnv::contains( const char* key ) const {
		return a->contains( key ) || b->contains( key );
	}
	bool MergeEnv::lookup( const char* key, ValuePtr* dst ) const {
		if ( a->lookup( key, dst ) ) return true;

		return b->lookup( key, dst );
	}

	EnvPtr TailEnv::merge( EnvPtr env )                      const { return env; }
	EnvPtr TailEnv::store( const char* key, ValuePtr val )   const { return new HeadEnv( key, val, this ); }
	bool   TailEnv::contains( const char* key )              const { return false; }
	bool   TailEnv::lookup( const char* key, ValuePtr* dst ) const { return false; }

	EnvPtr Env::make() { return new TailEnv(); }

	EnvPtr Env::store( SymbolPtr key, ValuePtr val )   const { return store( key->value, val );  }
	bool   Env::contains( SymbolPtr key )              const { return contains( key->value );    }
	bool   Env::lookup( SymbolPtr key, ValuePtr* dst ) const { return lookup( key->value, dst ); }

	EnvPtr Env::store( ValuePtr key, ValuePtr val )   const { return store( cast<Symbol>( key ), val );  }
	bool   Env::contains( ValuePtr key )              const { return contains( cast<Symbol>( key ) );    }
	bool   Env::lookup( ValuePtr key, ValuePtr* dst ) const { return lookup( cast<Symbol>( key ), dst ); }
}


