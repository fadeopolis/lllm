#ifndef __VALUE_HPP__
#define __VALUE_HPP__ 1

#include "lllm/predef.hpp"

#include <iosfwd>

namespace lllm {
	namespace value {
		class Value {
			public:
				static void* operator new( size_t size );
				static void* operator new( size_t size, void* mem );

				enum Type : long {
					#define LLLM_VISITOR( TYPE ) TYPE, 
					#include "lllm/values/Value_for_impl.inc"
					// values after Type::Lambda are also lambdas.
					// the arity of the lambda is (type - Type::Lambda).
					Lambda,
					// markers
					BEGIN = Nil,
					END   = Lambda
				};
			
				static bool isList( ValuePtr val );

				#define LLLM_VISITOR( TYPE ) static TYPE##Ptr as##TYPE( ValuePtr val );
				#include "lllm/values/Value_for_impl.inc"
				static NumberPtr asNumber( ValuePtr val );
				static LambdaPtr asLambda( ValuePtr val, size_t arity );

				const Type type;
			private:
				inline constexpr Value( Type t ) : type( t ) {}

			#define LLLM_VISITOR( TYPE ) friend class TYPE;
			#include "lllm/values/Value.inc"
		};
		class List : public Value {
			private:
				inline constexpr List( Type t ) : Value( t ) {}

			friend class Cons;
		};
		class Nil  : public List {
			private:
				Nil() = delete;
		};
		class Cons : public List {
			public:
				Cons( ValuePtr car, ListPtr cdr );				

				const ValuePtr car;
				const ListPtr  cdr;
		};
		class Number : public Value {
			private:
				inline constexpr Number( Type t ) : Value( t ) {}

			friend class Int;
			friend class Real;
		};
		class Int : public Number {
			public:
				Int();
				Int( long );
				
				const long value;
		};
		class Real : public Number {
			public:
				Real();
				Real( double );
				
				const double value;
		};
		class Char : public Value {
			public:
				Char( char );
				
				const char value;
		};
		class String : public Value {
			public:
				String( CStr );
				
				const CStr value;
		};
		class Symbol : public Value {
			public:
				Symbol( CStr );

				const CStr value;
		};
		class Ref : public Value {
			public:
				Ref();
				Ref( ValuePtr );

				ValuePtr get() const;
				ValuePtr set( ValuePtr ) const;
			private:
				mutable ValuePtr value;
		};
		class Lambda : public Value {
			public:
				typedef ValuePtr (*FnPtr)( LambdaPtr );

				static Lambda* alloc( size_t arity, size_t envSize );
	
				size_t arity() const;

				FnPtr                    fun;
				analyzer::ConstLambdaPtr ast;
				ValuePtr                 env[0];
			private:
				Lambda( size_t arity );
		};

		bool operator==( const Value&, const Value& );
		bool operator!=( const Value&, const Value& );
		bool equal( ValuePtr, ValuePtr );

		Value::Type typeOf( ValuePtr );

		NilPtr    nil();
		ConsPtr   cons( ValuePtr, ListPtr );
		IntPtr    number( int    );
		IntPtr    number( long   );
		RealPtr   number( float  );
		RealPtr   number( double );
		CharPtr   character( char );
		StringPtr string( CStr );
		SymbolPtr symbol( CStr );
		RefPtr    ref();
		RefPtr    ref( ValuePtr );

		inline ListPtr list() { return nil(); }
		template<typename... Tail>
		inline ListPtr list( ValuePtr v, Tail... tail ) {
			return cons( v, list( tail... ) );
		}
	
		template<typename Return, typename Visitor, typename... Args>
		Return visit( ValuePtr val, Visitor& v, Args&&... args ) {
			switch ( typeOf( val ) ) {
				#define LLLM_VISITOR( TYPE ) \
					case Value::Type::TYPE: return v.visit( static_cast<TYPE##Ptr>( val ), args... );
				#include "lllm/values/Value_for_impl.inc"
				default: return v.visit( static_cast<LambdaPtr>( val ), args... );
			}
		}
		template<typename Return, typename Visitor, typename... Args>
		Return visit( ValuePtr val, const Visitor& v, Args&&... args ) {
			switch ( typeOf( val ) ) {
				#define LLLM_VISITOR( TYPE ) \
					case Value::Type::TYPE: return v.visit( static_cast<TYPE##Ptr>( val ), args... );
				#include "lllm/values/Value_for_impl.inc"
				default: return v.visit( static_cast<LambdaPtr>( val ), args... );
			}
		}
/*
	struct Visitor final {
		X visit( IntPtr expr ) const {
			return;
		}
		X visit( RealPtr expr ) const {
			return;
		}
		X visit( CharPtr expr ) const {
			return;
		}
		X visit( StringPtr expr ) const {
			return;
		}
		X visit( SymbolPtr expr ) const {
			return;
		}
		X visit( ListPtr expr ) const {
			return;
		}
		X visit( RefPtr expr ) const {
			return;
		}
		X visit( LambdaPtr expr ) const {
			return;
		}
		X visit( ThunkPtr expr ) const {
			return;
		}
	};
*/
	}; // end namespace value
}; // end namespace lllm



#endif /* __VALUE_HPP__ */

