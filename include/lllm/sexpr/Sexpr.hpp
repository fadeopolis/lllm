#ifndef __SEXPR_HPP__
#define __SEXPR_HPP__ 1

#include "lllm/lllm.hpp"
#include "lllm/Obj.hpp"
#include "lllm/util/SourceLocation.hpp"
#include "lllm/util/InternedString.hpp"
#include "lllm/util/fail.hpp"

#include <vector>

namespace lllm {
	namespace sexpr {
		enum class Type {
			#define LLLM_VISIT( TYPE, ... )
			#define LLLM_VISIT_CONCRETE( TYPE, ... ) TYPE,
			#include "lllm/sexpr/Sexpr.inc"
		};

		typedef std::vector<SexprPtr>       SexprVector;
		typedef SexprVector::const_iterator SexprIterator;

		class Sexpr : public Obj {
			private:
				inline constexpr Sexpr( Type type, const util::SourceLocation& loc ) :
					location( loc ),
					type( type ) {}
			public:
				#define LLLM_VISIT( TYPE, ... )
				#define LLLM_VISIT_CONCRETE( TYPE, ... ) \
					bool      is##TYPE() const;          \
					TYPE##Ptr as##TYPE() const;
				#include "lllm/sexpr/Sexpr.inc"

				template<typename Return, typename Visitor, typename... Args>
				Return visit( Visitor&, Args... ) const;
				template<typename Return, typename Visitor, typename... Args>
				Return visit( const Visitor&, Args... ) const;

				const util::SourceLocation location;
			private:
				const Type type;

			#define LLLM_VISIT_ROOT( TYPE, ... )
			#define LLLM_VISIT( TYPE, ... ) friend class TYPE;
			#include "lllm/sexpr/Sexpr.inc"
		};

		#define LLLM_VISIT( TYPE, VALUE )
		#define LLLM_VISIT_CONCRETE( TYPE, VALUE )         \
			class TYPE : public Sexpr {                    \
				public:	                                   \
					TYPE( const util::SourceLocation& loc, \
					      const VALUE& value );            \
	                                                       \
					const VALUE value;                     \
			};
		#include "lllm/sexpr/Sexpr.inc"

		extern SexprPtr  nil;
		extern IntPtr    number( int );
		extern IntPtr    number( long );
		extern RealPtr   number( float );
		extern RealPtr   number( double );
		extern CharPtr   character( char );
		extern StringPtr string( util::CStr );
		extern SymbolPtr symbol( const util::InternedString& );
		extern ListPtr   list();
		extern ListPtr   cons( SexprPtr, ListPtr );

		template<typename T, typename... Ts>
		ListPtr list( T t, Ts... ts ) { return cons( t, list( ts... ) ); }

		extern size_t        length( ListPtr );
		extern SexprIterator begin( ListPtr );
		extern SexprIterator end( ListPtr );
		extern SexprPtr      at( ListPtr, size_t idx );
		extern SexprPtr      last( ListPtr );

		extern bool equal( SexprPtr, SexprPtr );

		template<typename Return, typename Visitor, typename... Args>
		Return Sexpr::visit( Visitor& v, Args... args ) const {
			switch ( type ) {
				#define LLLM_VISIT( TYPE, ... )
				#define LLLM_VISIT_CONCRETE( TYPE, ... ) \
					case Type::TYPE: return v.visit( static_cast<TYPE##Ptr>( this ), args... );
				#include "lllm/sexpr/Sexpr.inc"
			}
			LLLM_FAIL( "Invalid Sexpr tag " << ((unsigned) type) );
		}
		template<typename Return, typename Visitor, typename... Args>
		Return Sexpr::visit( const Visitor& v, Args... args ) const {
			switch ( type ) {
				#define LLLM_VISIT( TYPE, ... )
				#define LLLM_VISIT_CONCRETE( TYPE, ... ) \
					case Type::TYPE: return v.visit( static_cast<TYPE##Ptr>( this ), args... );
				#include "lllm/sexpr/Sexpr.inc"
			}
			LLLM_FAIL( "Invalid Sexpr tag " << ((unsigned) type) );
		}
	};

	extern bool operator==( const sexpr::Sexpr&, const sexpr::Sexpr& );
	extern bool operator!=( const sexpr::Sexpr&, const sexpr::Sexpr& );
};

#endif /* __SEXPR_HPP__ */
