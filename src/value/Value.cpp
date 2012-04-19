
#include "Value.hpp"
#include "ValueABI.hpp"

#include <cassert>

using namespace lllm;
using namespace lllm::val;

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
template<> RefPtr    lllm::cast<Ref>   ( ValuePtr v ) { return do_cast<Ref>   ( v ); }

template<> IntPtr    lllm::castOrNil<Int>   ( ValuePtr v ) { return do_cast_or_null<Int>   ( v ); }
template<> RealPtr   lllm::castOrNil<Real>  ( ValuePtr v ) { return do_cast_or_null<Real>  ( v ); }
template<> CharPtr   lllm::castOrNil<Char>  ( ValuePtr v ) { return do_cast_or_null<Char>  ( v ); }
template<> StringPtr lllm::castOrNil<String>( ValuePtr v ) { return do_cast_or_null<String>( v ); }
template<> SymbolPtr lllm::castOrNil<Symbol>( ValuePtr v ) { return do_cast_or_null<Symbol>( v ); }
template<> ConsPtr   lllm::castOrNil<Cons>  ( ValuePtr v ) { return do_cast_or_null<Cons>  ( v ); }
template<> LambdaPtr lllm::castOrNil<Lambda>( ValuePtr v ) { return do_cast_or_null<Lambda>( v ); }
template<> ThunkPtr  lllm::castOrNil<Thunk> ( ValuePtr v ) { return do_cast_or_null<Thunk> ( v ); }
template<> RefPtr    lllm::castOrNil<Ref>   ( ValuePtr v ) { return do_cast_or_null<Ref>   ( v ); }


