#ifndef __TEST_ABI_HPP__
#define __TEST_ABI_HPP__ 1

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

	namespace val {
		extern constexpr ValuePtr nil()   { return nullptr; }
		extern constexpr ValuePtr False() { return nullptr; }

		extern constexpr ValuePtr car( ConsPtr cons )  { return cons->car; }
		extern constexpr ValuePtr cdr( ConsPtr cons )  { return cons->cdr; }

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

#endif /* __TEST_ABI_HPP__ */

