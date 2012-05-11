#ifndef __VALUE_UTIL_HPP__
#define __VALUE_UTIL_HPP__ 1

#include "Value.hpp"

#include <utility>

namespace lllm {
	// a useful visitor template that saves us from writing lots of switch statements
	template<typename Visitor, typename Return, typename... Args>
	Return visit( ValuePtr val, Args... args ) {
		switch ( val::typeOf( val ) ) {
			#define CASE( TYPE ) \
				case Type::TYPE: \
					return Visitor::visit( static_cast<const TYPE*>( val ), args... );
			CASE( Int )
			CASE( Real )
			CASE( Char )
			CASE( String )
			CASE( Symbol )
			CASE( Cons )
			case Type::Nil: return Visitor::visit( (NilPtr) nullptr, args... ); break;
			CASE( Lambda )
			CASE( Thunk )
			CASE( Ref )
			#undef  CASE
		}
	}

	// default visitor that does nothing
	template<typename Return, typename... Args>
	struct Visitor {
		template<typename T>
		static Return visit( T v, Args... args ) { return Return(); }
/*		static Return visit( IntPtr    v, Args&&... args ) { return Return(); }
		static Return visit( RealPtr   v, Args&&... args ) { return Return(); }
		static Return visit( CharPtr   v, Args&&... args ) { return Return(); }
		static Return visit( StringPtr v, Args&&... args ) { return Return(); }
		static Return visit( SymbolPtr v, Args&&... args ) { return Return(); }
		static Return visit( ConsPtr   v, Args&&... args ) { return Return(); }
		static Return visit( NilPtr    v, Args&&... args ) { return Return(); }
		static Return visit( LambdaPtr v, Args&&... args ) { return Return(); }
		static Return visit( ThunkPtr  v, Args&&... args ) { return Return(); }
		static Return visit( RefPtr    v, Args&&... args ) { return Return(); }
*/	};
};

#endif /* __VALUE_UTIL_HPP__ */

