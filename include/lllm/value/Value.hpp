#ifndef __VALUE_HPP__
#define __VALUE_HPP__ 1

#include "lllm/lllm.hpp"
#include "lllm/util/InternedString.hpp"

#include <iosfwd>

namespace lllm {
	namespace value {
		extern unsigned long long cacheHits, cacheMisses;

		enum class Type : size_t {
			#define LLLM_VISIT( TYPE, ... )
			#define LLLM_VISIT_CONCRETE( TYPE, ... ) TYPE,
			#include "lllm/value/Value.inc"

			// values after Type::Lambda are also lambdas.
			// the arity of the lambda is (type - Type::Lambda).
			Lambda,

			// markers
			BEGIN = Nil,
			END   = Lambda
		};

		Type typeOf( ValuePtr );

		class Value : public gc::gc {
			public:
				static bool isList( ValuePtr val );
				static bool isLambda( ValuePtr val );

				#define LLLM_VISIT( TYPE )
				#define LLLM_VISIT_CONCRETE( TYPE ) static TYPE##Ptr as##TYPE( ValuePtr val );
				#include "lllm/value/Value.inc"

				static NumberPtr asNumber( ValuePtr val );
				static LambdaPtr asLambda( ValuePtr val );
				static LambdaPtr asLambda( ValuePtr val, size_t arity );
			private:
				inline constexpr Value( Type t ) : type( t ) {}

				const Type type;

			#define LLLM_VISIT_ROOT( TYPE )
			#define LLLM_VISIT( TYPE ) friend class TYPE;
			#include "lllm/value/Value.inc"

			friend Type typeOf( ValuePtr );
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
				String( util::CStr );

				const util::CStr value;
		};
		class Symbol : public Value {
			public:
				Symbol( const util::InternedString& );

				const util::InternedString& value;
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

				struct Data {
					inline constexpr Data( ast::LambdaPtr ast ) : callCnt( 0 ), code( nullptr ), ast( ast ) {}

					size_t         callCnt;
					FnPtr          code;
					ast::LambdaPtr ast;
				};
				typedef Data* DataPtr;

				static Lambda* alloc( ast::LambdaPtr ast );
				static Lambda* alloc( ast::LambdaPtr ast, FnPtr code );
				static Lambda* alloc( size_t arity, size_t envSize, FnPtr code );

				size_t arity() const;

				mutable FnPtr   code;
				const   DataPtr data;
				ValuePtr        env[0];
			private:
				Lambda( size_t arity, Data* data, FnPtr code );
		};

		bool equal( ValuePtr, ValuePtr );

		extern NilPtr    nil;
		extern ValuePtr  True();
		extern ValuePtr  False;
		extern ConsPtr   cons( ValuePtr, ListPtr );
		extern IntPtr    number( int    );
		extern IntPtr    number( long   );
		extern RealPtr   number( float  );
		extern RealPtr   number( double );
		extern CharPtr   character( char );
		extern StringPtr string( util::CStr );
		extern SymbolPtr symbol( const util::InternedString& );
		extern RefPtr    ref();
		extern RefPtr    ref( ValuePtr );

		inline ListPtr list() { return nil; }
		template<typename... Tail>
		inline ListPtr list( ValuePtr v, Tail... tail ) {
			return cons( v, list( tail... ) );
		}

		template<typename Return, typename Visitor, typename... Args>
		Return visit( ValuePtr val, Visitor& v, Args&&... args ) {
			switch ( value::typeOf( val ) ) {
				#define LLLM_VISIT( TYPE )
				#define LLLM_VISIT_CONCRETE( TYPE ) \
					case value::Type::TYPE: return v.visit( static_cast<TYPE##Ptr>( val ), args... );
				#include "lllm/value/Value.inc"
				default: return v.visit( static_cast<LambdaPtr>( val ), args... );
			}
		}
		template<typename Return, typename Visitor, typename... Args>
		Return visit( ValuePtr val, const Visitor& v, Args&&... args ) {
			switch ( value::typeOf( val ) ) {
				#define LLLM_VISIT( TYPE )
				#define LLLM_VISIT_CONCRETE( TYPE ) \
					case value::Type::TYPE: return v.visit( static_cast<TYPE##Ptr>( val ), args... );
				#include "lllm/value/Value.inc"
				default: return v.visit( static_cast<LambdaPtr>( val ), args... );
			}
		}

		static_assert( sizeof( Value ) == 8, "Value must be 8 bytes in size" );
	};

	bool operator==( const value::Value&, const value::Value& );
	bool operator!=( const value::Value&, const value::Value& );
};

#endif /* __VALUE_HPP__ */
