#ifndef __VALUE_UTIL_HPP__
#define __VALUE_UTIL_HPP__ 1

#include "Value.hpp"
#include "ValueABI.hpp"

namespace lllm {
	// a useful visitor template that saves us from writing lots of switch statements
	template<typename Visitor, typename Return, typename... Args>
	Return visit( ValuePtr val, Visitor& v, Args&&... args ) {
		switch ( val::typeOf( val ) ) {
			#define CASE( TYPE ) case Type::TYPE: return v( cast<TYPE>( val ), std::forward<Args>(args)... );
			CASE( Int )
			CASE( Real )
			CASE( Char )
			CASE( String )
			CASE( Symbol )
			CASE( Cons )
			case Type::Nil: return v( (NilPtr) nullptr, std::forward<Args>(args)... ); break;
			CASE( Lambda )
			CASE( Thunk )
			#undef  CASE
		}
	}

	// default visitor that does nothing
	template<typename Return, typename... Args>
	struct Visitor {
		Return operator()( IntPtr    v, Args&&... args ) {}
		Return operator()( RealPtr   v, Args&&... args ) {}
		Return operator()( CharPtr   v, Args&&... args ) {}
		Return operator()( StringPtr v, Args&&... args ) {}
		Return operator()( SymbolPtr v, Args&&... args ) {}
		Return operator()( ConsPtr   v, Args&&... args ) {}
		Return operator()( NilPtr    v, Args&&... args ) {}
		Return operator()( LambdaPtr v, Args&&... args ) {}
		Return operator()( ThunkPtr  v, Args&&... args ) {}
	};
};

#endif /* __VALUE_UTIL_HPP__ */

