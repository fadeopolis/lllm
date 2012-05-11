#ifndef __TYPE_SET_HPP__
#define __TYPE_SET_HPP__ 1

#include "lllm/value/Value.hpp"

namespace lllm {
	namespace util {
		static_assert( (sizeof( short ) * 8) >= long(value::Type::END), "TypeSet cannot be packed into a short, it's too small" );

		class TypeSet;

		// type set union
		inline constexpr TypeSet operator|( const value::Type& a, const value::Type& b );
		inline constexpr TypeSet operator|( const TypeSet&     a, const TypeSet&     b );
		inline constexpr TypeSet operator|( const value::Type& a, const TypeSet&     b );
		inline constexpr TypeSet operator|( const TypeSet&     a, const value::Type& b );
		// type set intersection
		inline constexpr TypeSet operator&( const value::Type& a, const value::Type& b );
		inline constexpr TypeSet operator&( const TypeSet&     a, const TypeSet&     b );
		inline constexpr TypeSet operator&( const value::Type& a, const TypeSet&     b );
		inline constexpr TypeSet operator&( const TypeSet&     a, const value::Type& b );

		inline constexpr bool operator==( const TypeSet&, const TypeSet& );

		class TypeSet {
			public:
				#define LLLM_VISITOR( TYPE ) \
					static inline constexpr TypeSet TYPE() { return TypeSet( value::Type::TYPE ); }
				#include "lllm/value/Value_concrete.inc"

				static inline constexpr TypeSet Number() { return Int() | Real(); }	
				static inline constexpr TypeSet Lambda() { return TypeSet( value::Type::Lambda ); }	
				static inline constexpr TypeSet all()    { return all( TypeSet(), value::Type::BEGIN ); }
	
				inline constexpr bool contains( const value::Type& t ) const { return (bits( t ) & mask) != 0; }

				inline unsigned size() const { return __builtin_popcount( mask ); }

				inline constexpr TypeSet()                       : TypeSet( 0 )         {}
				inline constexpr TypeSet( const value::Type& t ) : TypeSet( bits( t ) ) {}
				inline constexpr TypeSet( const TypeSet&     t ) : TypeSet( t.mask )    {}

				inline TypeSet& operator=( const TypeSet& ts ) {
					mask = ts.mask;
					return *this;
				}
			private:
				inline constexpr TypeSet( short mask ) : mask( mask ) {}

				short mask;

				static inline constexpr short bits()                       { return 0; }
				static inline constexpr short bits( value::Type t ) { 
					return 1 << ((short) (t > value::Type::Lambda ? value::Type::Lambda : t)); 
				}
				static inline constexpr short bits( const TypeSet& t )     { return t.mask; }
				template<typename... Ts>
				static inline constexpr short bits( value::Type t, Ts... ts ) { return bits( t ) | bits( ts... ); }

				static inline constexpr TypeSet all( const TypeSet& accum, value::Type t ) {
					return t <= value::Type::END ? all( accum | t, value::Type( short( t ) + 1 ) ) : accum; 
				}

			friend constexpr TypeSet operator|( const value::Type& a, const value::Type& b );
			friend constexpr TypeSet operator|( const TypeSet&     a, const TypeSet&     b );
			friend constexpr TypeSet operator|( const value::Type& a, const TypeSet&     b );
			friend constexpr TypeSet operator|( const TypeSet&     a, const value::Type& b );
			friend constexpr TypeSet operator&( const value::Type& a, const value::Type& b );
			friend constexpr TypeSet operator&( const TypeSet&     a, const TypeSet&     b );
			friend constexpr TypeSet operator&( const value::Type& a, const TypeSet&     b );
			friend constexpr TypeSet operator&( const TypeSet&     a, const value::Type& b );

			friend constexpr bool operator==( const TypeSet&, const TypeSet& );
		};

		// type set union
		constexpr TypeSet operator|( const value::Type& a, const value::Type& b ) { return TypeSet( TypeSet::bits( a ) | TypeSet::bits( b ) ); }
		constexpr TypeSet operator|( const TypeSet&     a, const TypeSet&     b ) { return TypeSet( TypeSet::bits( a ) | TypeSet::bits( b ) ); }
		constexpr TypeSet operator|( const value::Type& a, const TypeSet&     b ) { return TypeSet( TypeSet::bits( a ) | TypeSet::bits( b ) ); }
		constexpr TypeSet operator|( const TypeSet&     a, const value::Type& b ) { return TypeSet( TypeSet::bits( a ) | TypeSet::bits( b ) ); }
		// type set intersection
		constexpr TypeSet operator&( const value::Type& a, const value::Type& b ) { return TypeSet( TypeSet::bits( a ) & TypeSet::bits( b ) ); }
		constexpr TypeSet operator&( const TypeSet&     a, const TypeSet&     b ) { return TypeSet( TypeSet::bits( a ) & TypeSet::bits( b ) ); }
		constexpr TypeSet operator&( const value::Type& a, const TypeSet&     b ) { return TypeSet( TypeSet::bits( a ) & TypeSet::bits( b ) ); }
		constexpr TypeSet operator&( const TypeSet&     a, const value::Type& b ) { return TypeSet( TypeSet::bits( a ) & TypeSet::bits( b ) ); }

		constexpr bool operator==( const TypeSet& a, const TypeSet& b ) { return a.mask == b.mask; }		
	};
};


#endif /* __TYPE_SET_HPP__ */

