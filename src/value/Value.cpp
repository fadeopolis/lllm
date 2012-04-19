
#include "Value.hpp"
#include "ValueABI.hpp"

#include <cassert>

using namespace lllm;
using namespace lllm::val;

IntPtr lllm::val::integer( long value ) {
	return new Int( value );
}
ConsPtr lllm::val::cons( ValuePtr car, ValuePtr cdr ) {
	return new Cons( car, cdr );
}

ValuePtr val::cadr( ConsPtr cons ) { return car( cast<Cons>( cdr( cons ) ) ); }
ValuePtr val::cdar( ConsPtr cons ) { return cdr( cast<Cons>( car( cons ) ) ); }

		extern ValuePtr value( int         v );
		extern ValuePtr value( long        v );
		extern ValuePtr value( double      v );

ValuePtr val::value( int    v ) { return new Int( v ); }
ValuePtr val::value( long   v ) { return new Int( v ); }
ValuePtr val::value( double v ) { return new Real( v ); }

template<typename T>
inline const T* do_cast( ValuePtr v ) {
	assert( typeOf( v ) == T::TYPE );
	
	return static_cast<const T*>( v );
}
template<>
inline NilPtr do_cast( ValuePtr v ) {
	assert( v == val::nil() );
	
	return nullptr;
}

template<typename T>
inline const T* do_cast_or_null( ValuePtr v ) {
	return typeOf( v ) == T::TYPE ? static_cast<const T*>( v ) : nullptr;
}
template<>
inline NilPtr do_cast_or_null( ValuePtr v ) {
	return nullptr;
}

template<> IntPtr    lllm::cast<Int>   ( ValuePtr v ) { return do_cast<Int>   ( v ); }
template<> RealPtr   lllm::cast<Real>  ( ValuePtr v ) { return do_cast<Real>  ( v ); }
template<> CharPtr   lllm::cast<Char>  ( ValuePtr v ) { return do_cast<Char>  ( v ); }
template<> StringPtr lllm::cast<String>( ValuePtr v ) { return do_cast<String>( v ); }
template<> SymbolPtr lllm::cast<Symbol>( ValuePtr v ) { return do_cast<Symbol>( v ); }
template<> ConsPtr   lllm::cast<Cons>  ( ValuePtr v ) { return do_cast<Cons>  ( v ); }
template<> LambdaPtr lllm::cast<Lambda>( ValuePtr v ) { return do_cast<Lambda>( v ); }
template<> ThunkPtr  lllm::cast<Thunk> ( ValuePtr v ) { return do_cast<Thunk> ( v ); }

template<> IntPtr    lllm::castOrNull<Int>   ( ValuePtr v ) { return do_cast_or_null<Int>   ( v ); }
template<> RealPtr   lllm::castOrNull<Real>  ( ValuePtr v ) { return do_cast_or_null<Real>  ( v ); }
template<> CharPtr   lllm::castOrNull<Char>  ( ValuePtr v ) { return do_cast_or_null<Char>  ( v ); }
template<> StringPtr lllm::castOrNull<String>( ValuePtr v ) { return do_cast_or_null<String>( v ); }
template<> SymbolPtr lllm::castOrNull<Symbol>( ValuePtr v ) { return do_cast_or_null<Symbol>( v ); }
template<> ConsPtr   lllm::castOrNull<Cons>  ( ValuePtr v ) { return do_cast_or_null<Cons>  ( v ); }
template<> LambdaPtr lllm::castOrNull<Lambda>( ValuePtr v ) { return do_cast_or_null<Lambda>( v ); }
template<> ThunkPtr  lllm::castOrNull<Thunk> ( ValuePtr v ) { return do_cast_or_null<Thunk> ( v ); }


