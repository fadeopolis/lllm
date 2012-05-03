#ifndef __EVN_HPP__
#define __EVN_HPP__ 1

#include "lllm/predef.hpp"

namespace lllm {
	namespace eval {
		struct Binding {
			inline constexpr Binding()                                : Binding( nullptr, nullptr ) {}
			inline constexpr Binding( CStr key, value::ValuePtr val ) : key( key ), val( val )      {}
	
			const CStr            key;
			const value::ValuePtr val;
		};

		class Env {
			public:
				static EnvPtr make( CStr key, value::ValuePtr val );
	
				bool            contains( CStr key ) const;			
				value::ValuePtr get( CStr key ) const;

				EnvPtr put( CStr key, value::ValuePtr val ) const;
				EnvPtr put( const Binding& ) const;

				const Binding binding;
				const EnvPtr  parent;
			private:
				Env( EnvPtr parent, CStr key, value::ValuePtr val );
		};		
	}; // end namespace analyzer
}; // end namespace lllm


#endif /* __EVN_HPP__ */

