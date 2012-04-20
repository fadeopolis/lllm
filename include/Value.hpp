#ifndef __TEST_HPP__
#define __TEST_HPP__ 1

namespace lllm {
	typedef const class Value*  ValuePtr;

	namespace val {
		// *** constants
		extern constexpr ValuePtr nil() { return nullptr; }
		extern const ValuePtr True;
		extern const ValuePtr False;
		// *** construtors
		extern ValuePtr number( int );
		extern ValuePtr number( long );
		extern ValuePtr number( double );
		extern ValuePtr character( char );
		extern ValuePtr string( const char* );
		extern ValuePtr symbol( const char* );
		extern ValuePtr cons( ValuePtr car, ValuePtr cdr );
		extern ValuePtr ref();
		extern ValuePtr ref( ValuePtr v );
		// *** destructuring
		extern ValuePtr car( ValuePtr cons );
		extern ValuePtr cdr( ValuePtr cons );

		extern ValuePtr cadr( ValuePtr cons );
		extern ValuePtr cdar( ValuePtr cons );

		extern ValuePtr cadar( ValuePtr cons );
		extern ValuePtr caddr( ValuePtr cons );

		// *** predicates
		// ** type checks
		extern constexpr bool isInt   ( ValuePtr );
		extern constexpr bool isReal  ( ValuePtr );
		extern constexpr bool isChar  ( ValuePtr );
		extern constexpr bool isString( ValuePtr );
		extern constexpr bool isSymbol( ValuePtr );
		extern constexpr bool isCons  ( ValuePtr );
		extern constexpr bool isNil   ( ValuePtr );
		extern constexpr bool isLambda( ValuePtr );
		extern constexpr bool isThunk ( ValuePtr );
		extern constexpr bool isRef   ( ValuePtr );
		// ** misc
		extern bool equal( ValuePtr, ValuePtr );
	}

/*
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
	class Ref;

	typedef const Value*  ValuePtr;
	typedef const Int*    IntPtr;
	typedef const Real*   RealPtr;
	typedef const Char*   CharPtr;
	typedef const String* StringPtr;
	typedef const Symbol* SymbolPtr;
	typedef const Cons*   ConsPtr;
	typedef const Nil*    NilPtr;
	typedef const Lambda* LambdaPtr;
	typedef const Thunk*  ThunkPtr;
	typedef const Ref*    RefPtr;

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
	template<>           RefPtr    cast<Ref>   ( ValuePtr v );

	template<typename T> const T*  castOrNil( ValuePtr v );
	template<>           IntPtr    castOrNil<Int>   ( ValuePtr v );
	template<>           RealPtr   castOrNil<Real>  ( ValuePtr v );
	template<>           CharPtr   castOrNil<Char>  ( ValuePtr v );
	template<>           StringPtr castOrNil<String>( ValuePtr v );
	template<>           SymbolPtr castOrNil<Symbol>( ValuePtr v );
	template<>           ConsPtr   castOrNil<Cons>  ( ValuePtr v );
	template<>           LambdaPtr castOrNil<Lambda>( ValuePtr v );
	template<>           ThunkPtr  castOrNil<Thunk> ( ValuePtr v );
	template<>           RefPtr    castOrNil<Ref>   ( ValuePtr v );

	// builtin functions

	namespace val {
		// *** constants
		extern constexpr ValuePtr nil();
		extern constexpr ValuePtr True();
		extern constexpr ValuePtr False();
		// *** construtors
		extern IntPtr    integer( long );
		extern RealPtr   real( double );
		extern CharPtr   character( char );
		extern StringPtr string( const char* );
		extern SymbolPtr symbol( const char* );
		extern ConsPtr   cons( ValuePtr car, ValuePtr cdr );
		extern RefPtr    ref();
		extern RefPtr    ref( ValuePtr v );
		// *** destructuring
		extern constexpr ValuePtr car( ConsPtr cons );
		extern constexpr ValuePtr cdr( ConsPtr cons );
		extern ValuePtr cadr( ConsPtr cons );
		extern ValuePtr cdar( ConsPtr cons );
		// *** predicates
		// ** type checks
		extern constexpr bool isInt( ValuePtr );
		extern constexpr bool isReal( ValuePtr );
		extern constexpr bool isChar( ValuePtr );
		extern constexpr bool isString( ValuePtr );
		extern constexpr bool isSymbol( ValuePtr );
		extern constexpr bool isCons( ValuePtr );
		extern constexpr bool isNil( ValuePtr );
		extern constexpr bool isLambda( ValuePtr );
		extern constexpr bool isThunk( ValuePtr );
		extern constexpr bool isRef( ValuePtr );
		// ** misc
		extern bool equal( ValuePtr, ValuePtr );
	}
*/

}

#endif /* __TEST_HPP__ */

