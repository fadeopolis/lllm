#ifndef __VALUE_INTERNALS_HPP__
#define __VALUE_INTERNALS_HPP__ 1

#include "Value.hpp"

namespace lllm {
	enum class Type : long {
		// readable types
		Int,
		Real,
		Char,
		String,
		Symbol,
		Cons,
		// non-readable types
		Nil,
		Lambda,
		Thunk,
		Ref
	};

	typedef const struct Int*    IntPtr;
	typedef const struct Real*   RealPtr;
	typedef const struct Char*   CharPtr;
	typedef const struct String* StringPtr;
	typedef const struct Symbol* SymbolPtr;
	typedef const struct Cons*   ConsPtr;
	typedef const struct Nil*    NilPtr;
	typedef const struct Lambda* LambdaPtr;
	typedef const struct Thunk*  ThunkPtr;
	typedef const struct Ref*    RefPtr;

	struct Value {
		constexpr Value( Type type ) : type( type ) {}

		const Type type;
	};
	struct Int : public Value {
		static constexpr Type TYPE = Type::Int;

		constexpr Int( long value ) : Value( Type::Int ), value( value ) {}

		const long value;	
	};
	struct Real : public Value {
		static constexpr Type TYPE = Type::Real;

		constexpr Real( double value ) : Value( Type::Real ), value( value ) {}

		const double value;
	};
	struct Char : public Value {
		static constexpr Type TYPE = Type::Char;

		constexpr Char( char value ) : Value( Type::Char ), value( value ) {}

		const char value;
	};
	struct String : public Value {
		static constexpr Type TYPE = Type::String;

		constexpr String( const char* value ) : Value( Type::String ), value( value ) {}

		const char* const value;
	};
	struct Symbol : public Value {
		static constexpr Type TYPE = Type::Symbol;

		constexpr Symbol( const char* value ) : Value( Type::Symbol ), value( value ) {}

		const char* const value;
	};
	struct Cons : public Value {
		static constexpr Type TYPE = Type::Cons;

		constexpr Cons( ValuePtr car, ValuePtr cdr ) : Value( Type::Cons ), car( car ), cdr( cdr ) {}

		const ValuePtr car, cdr;
	};
	struct Lambda : public Value {
		static constexpr Type TYPE = Type::Lambda;
	};
	struct Thunk : public Value {
		static constexpr Type TYPE = Type::Thunk;
	};
	struct Ref : public Value {
		static constexpr Type TYPE = Type::Ref;

		constexpr Ref()             : Ref( nullptr )                 {}
		constexpr Ref( ValuePtr v ) : Value( Type::Ref ), value( v ) {}

		ValuePtr value;
	};

	// checked casts
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

	namespace val {
		extern constexpr Type typeOf( ValuePtr v ) {
			return v ? v->type : Type::Nil;
		}

		extern constexpr bool isInt( ValuePtr v )    { return v && v->type == Type::Int;    }
		extern constexpr bool isReal( ValuePtr v )   { return v && v->type == Type::Real;   }
		extern constexpr bool isChar( ValuePtr v )   { return v && v->type == Type::Char;   }
		extern constexpr bool isString( ValuePtr v ) { return v && v->type == Type::String; }
		extern constexpr bool isSymbol( ValuePtr v ) { return v && v->type == Type::Symbol; }
		extern constexpr bool isCons( ValuePtr v )   { return v && v->type == Type::Cons;   }
		extern constexpr bool isNil( ValuePtr v )    { return !v;                           }
		extern constexpr bool isLambda( ValuePtr v ) { return v && v->type == Type::Lambda; }
		extern constexpr bool isThunk( ValuePtr v )  { return v && v->type == Type::Thunk;  }
		extern constexpr bool isRef( ValuePtr v )    { return v && v->type == Type::Ref;    }
	}
}

#endif /* __VALUE_INTERNALS_HPP__ */

