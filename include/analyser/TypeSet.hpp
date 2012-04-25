#ifndef __TYPE_SET_HPP__
#define __TYPE_SET_HPP__ 1

#include "values/Value.hpp"

#include <iosfwd>

namespace lllm {
	static_assert( (sizeof( short ) * 8) >= long(Type::END), "TypeSet cannot be packed into a short, it's too small" );

	class TypeSet;

	std::ostream& operator<<( std::ostream&, const TypeSet& );	

	// type set union
	inline constexpr TypeSet operator|( const Type&    a, const Type&    b );
	inline constexpr TypeSet operator|( const TypeSet& a, const TypeSet& b );
	inline constexpr TypeSet operator|( const Type&    a, const TypeSet& b );
	inline constexpr TypeSet operator|( const TypeSet& a, const Type&    b );
	// type set intersection
	inline constexpr TypeSet operator&( const Type&    a, const Type&    b );
	inline constexpr TypeSet operator&( const TypeSet& a, const TypeSet& b );
	inline constexpr TypeSet operator&( const Type&    a, const TypeSet& b );
	inline constexpr TypeSet operator&( const TypeSet& a, const Type&    b );

	inline constexpr bool operator==( const TypeSet&, const TypeSet& );

	class TypeSet {
		public:
			static inline constexpr TypeSet Nil()    { return TypeSet( Type::Nil    ); }
			static inline constexpr TypeSet Cons()   { return TypeSet( Type::Cons   ); }
			static inline constexpr TypeSet Int()    { return TypeSet( Type::Int    ); }
			static inline constexpr TypeSet Real()   { return TypeSet( Type::Real   ); }
			static inline constexpr TypeSet Char()   { return TypeSet( Type::Char   ); }
			static inline constexpr TypeSet String() { return TypeSet( Type::String ); }
			static inline constexpr TypeSet Symbol() { return TypeSet( Type::Symbol ); }
			static inline constexpr TypeSet Ref()    { return TypeSet( Type::Ref    ); }
			static inline constexpr TypeSet Lambda() { return TypeSet( Type::Lambda ); }
			static inline constexpr TypeSet Thunk()  { return TypeSet( Type::Thunk  ); }

			static inline constexpr TypeSet all() { return all( TypeSet(), Type::BEGIN ); }

			inline constexpr bool contains( const Type& t ) const { return (bits( t ) & mask) != 0; }

			inline unsigned size() const { return __builtin_popcount( mask ); }

			inline constexpr TypeSet()                   : TypeSet( 0 )         {}
			inline constexpr TypeSet( const Type& t )    : TypeSet( bits( t ) ) {}
			inline constexpr TypeSet( const TypeSet& t ) : TypeSet( t.mask )    {}
//		private:
			inline constexpr TypeSet( short mask ) : mask( mask ) {}

			const short mask;

			static inline constexpr short bits()                   { return 0; }
			static inline constexpr short bits( Type t )           { return 1 << ((short) t); }
			static inline constexpr short bits( const TypeSet& t ) { return t.mask; }
			template<typename... Ts>
			static inline constexpr short bits( Type t, Ts... ts ) { return bits( t ) | bits( ts... ); }

			static inline constexpr TypeSet all( const TypeSet& accum, Type t ) {
				return t <= Type::END ? all( accum | t, Type( short( t ) + 1 ) ) : accum; 
			}

		friend std::ostream& operator<<( std::ostream&, const TypeSet& );
		friend constexpr TypeSet operator|( const Type&    a, const Type&    b );
		friend constexpr TypeSet operator|( const TypeSet& a, const TypeSet& b );
		friend constexpr TypeSet operator|( const Type&    a, const TypeSet& b );
		friend constexpr TypeSet operator|( const TypeSet& a, const Type&    b );
		friend constexpr TypeSet operator&( const Type&    a, const Type&    b );
		friend constexpr TypeSet operator&( const TypeSet& a, const TypeSet& b );
		friend constexpr TypeSet operator&( const Type&    a, const TypeSet& b );
		friend constexpr TypeSet operator&( const TypeSet& a, const Type&    b );
		friend constexpr bool operator==( const TypeSet&, const TypeSet& );
	};

	// type set union
	constexpr TypeSet operator|( const Type&    a, const Type&    b ) { return TypeSet( TypeSet::bits( a ) | TypeSet::bits( b ) ); }
	constexpr TypeSet operator|( const TypeSet& a, const TypeSet& b ) { return TypeSet( TypeSet::bits( a ) | TypeSet::bits( b ) ); }
	constexpr TypeSet operator|( const Type&    a, const TypeSet& b ) { return TypeSet( TypeSet::bits( a ) | TypeSet::bits( b ) ); }
	constexpr TypeSet operator|( const TypeSet& a, const Type&    b ) { return TypeSet( TypeSet::bits( a ) | TypeSet::bits( b ) ); }
	// type set intersection
	constexpr TypeSet operator&( const Type&    a, const Type&    b ) { return TypeSet( TypeSet::bits( a ) & TypeSet::bits( b ) ); }
	constexpr TypeSet operator&( const TypeSet& a, const TypeSet& b ) { return TypeSet( TypeSet::bits( a ) & TypeSet::bits( b ) ); }
	constexpr TypeSet operator&( const Type&    a, const TypeSet& b ) { return TypeSet( TypeSet::bits( a ) & TypeSet::bits( b ) ); }
	constexpr TypeSet operator&( const TypeSet& a, const Type&    b ) { return TypeSet( TypeSet::bits( a ) & TypeSet::bits( b ) ); }

	constexpr bool operator==( const TypeSet& a, const TypeSet& b ) { return a.mask == b.mask; }
};


#endif /* __TYPE_SET_HPP__ */

