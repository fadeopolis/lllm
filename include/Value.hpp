#ifndef __TEST_HPP__
#define __TEST_HPP__ 1

namespace lllm {
	class Value;
	class Int;
	class Real;
	class Char;
	class String;
	class Symbol;
	class Cons;
	class Nil;
	class Lambda;
	class Thunk;

	typedef const Value*  ValuePtr;
	typedef const Int*    IntPtr;
	typedef const Real*   RealPtr;
	typedef const Char*   CharPtr;
	typedef const String* StringPtr;
	typedef const Symbol* SymbolPtr;
	typedef const Cons*   ConsPtr;
	typedef const Nil*    NilPtr;
	typedef const Lambda* LambdaPtr;
	typedef const Thunk * ThunkPtr;

	namespace val {
		// constants
		extern constexpr ValuePtr nil();
		// construtors
		extern IntPtr    integer( long );
		extern RealPtr   real( double );
		extern CharPtr   character( double );
		extern StringPtr string( const char* );
		extern SymbolPtr symbol( const char* );
		extern ConsPtr   cons( ValuePtr car, ValuePtr cdr );
		// destructuring
		extern constexpr ValuePtr car( ConsPtr cons );
		extern constexpr ValuePtr cdr( ConsPtr cons );
		extern ValuePtr cadr( ConsPtr cons );
		extern ValuePtr cdar( ConsPtr cons );

		// predicates
		extern constexpr bool isInt( ValuePtr );
		extern constexpr bool isReal( ValuePtr );
		extern constexpr bool isChar( ValuePtr );
		extern constexpr bool isString( ValuePtr );
		extern constexpr bool isSymbol( ValuePtr );
		extern constexpr bool isCons( ValuePtr );
		extern constexpr bool isNil( ValuePtr );
		extern constexpr bool isLambda( ValuePtr );
		extern constexpr bool isThunk( ValuePtr );

		// useful helpers
		extern constexpr ValuePtr value( ValuePtr v ) { return v; } 
		extern ValuePtr value( int         v );
		extern ValuePtr value( long        v );
		extern ValuePtr value( double      v );

		extern constexpr ValuePtr list() { return nullptr; }

		template<typename Car, typename... Cdr>
		ConsPtr list( Car car, Cdr... cdr ) {
			return cons( value( car ), list( cdr... ) );
		}
	}

	// hack around the fact that forward decls do not allow for inheritance
	template<typename T> constexpr Value* upcast( T* t ) { return (Value*) t; }

	template<typename T> const T*  cast( ValuePtr v );
	template<>           IntPtr    cast<Int>   ( ValuePtr v );
	template<>           RealPtr   cast<Real>  ( ValuePtr v );
	template<>           CharPtr   cast<Char>  ( ValuePtr v );
	template<>           StringPtr cast<String>( ValuePtr v );
	template<>           SymbolPtr cast<Symbol>( ValuePtr v );
	template<>           ConsPtr   cast<Cons>  ( ValuePtr v );
	template<>           LambdaPtr cast<Lambda>( ValuePtr v );
	template<>           ThunkPtr  cast<Thunk> ( ValuePtr v );

	template<typename T> const T*  castOrNull( ValuePtr v );
	template<>           IntPtr    castOrNull<Int>   ( ValuePtr v );
	template<>           RealPtr   castOrNull<Real>  ( ValuePtr v );
	template<>           CharPtr   castOrNull<Char>  ( ValuePtr v );
	template<>           StringPtr castOrNull<String>( ValuePtr v );
	template<>           SymbolPtr castOrNull<Symbol>( ValuePtr v );
	template<>           ConsPtr   castOrNull<Cons>  ( ValuePtr v );
	template<>           LambdaPtr castOrNull<Lambda>( ValuePtr v );
	template<>           ThunkPtr  castOrNull<Thunk> ( ValuePtr v );
}

#endif /* __TEST_HPP__ */

