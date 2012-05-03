#ifndef __SEXPR_HPP__
#define __SEXPR_HPP__ 1

#include "lllm/predef.hpp"
#include "lllm/reader/SourceLocation.hpp"
#include "lllm/util/InternedString.hpp"

#include <vector>

// an immutable representation of program text

namespace lllm {
	namespace reader {
		typedef std::vector<Sexpr> Sexprs;

		class Sexpr {
			private:
				enum class Type {
					#define LLLM_VISITOR( TYPE ) TYPE, 
					#include "lllm/reader/Sexpr.inc"
				};
			public:
				template<typename Return, typename Visitor, typename... Args>
				Return visit( Visitor&, Args... ) const;
				template<typename Return, typename Visitor, typename... Args>
				Return visit( const Visitor&, Args... ) const;
	
				#define LLLM_VISITOR( TYPE ) bool is##TYPE() const;
				#include "lllm/reader/Sexpr.inc"

				#define LLLM_VISITOR( TYPE ) TYPE##Ptr as##TYPE() const;
				#include "lllm/reader/Sexpr.inc"

				const SourceLocation location;
			private:
				Sexpr( Type, const SourceLocation& );

				const Type type;
	
			#define LLLM_VISITOR( TYPE ) friend class TYPE; 
			#include "lllm/reader/Sexpr.inc"
		};
		class Int final : public Sexpr {
			public:
				Int( const SourceLocation&, long );
	
				const long value;
		};
		class Real final : public Sexpr {
			public:
				Real( const SourceLocation&, double );
	
				const double value;
		};
		class Char final : public Sexpr {
			public:
				Char( const SourceLocation&, char );
	
				const char value;
		};
		class String final : public Sexpr {
			public:
				String( const SourceLocation&, CStr );
	
				const CStr value;
		};
		class Symbol final : public Sexpr {
			public:
				Symbol( const SourceLocation&, CStr );
				Symbol( const SourceLocation&, util::InternedString );
	
				const util::InternedString value;
		};
		class List final : public Sexpr {
			public:
				typedef std::vector<SexprPtr>::const_iterator iterator;

				List( const SourceLocation& );
				List( const SourceLocation&, const std::vector<SexprPtr>& exprs );
	
				SexprPtr operator[]( size_t idx ) const;
				SexprPtr at( size_t idx ) const;

				iterator begin() const;
				iterator end()   const;

				size_t length() const;
			private:
				const std::vector<SexprPtr> exprs;
		};
	
		bool operator==( SexprRef, SexprRef );
		bool operator!=( SexprRef, SexprRef );

		bool equal( SexprPtr, SexprPtr );

		SexprPtr  nil();
		IntPtr    number( int );
		IntPtr    number( long );
		RealPtr   number( float );
		RealPtr   number( double );
		CharPtr   character( char );
		StringPtr string( CStr );
		SymbolPtr symbol( CStr );
		ListPtr   list();
		ListPtr   cons( SexprPtr, ListPtr );
	
		template<typename T, typename... Ts>
		ListPtr list( T t, Ts... ts ) { return cons( t, list( ts... ) ); }

		template<typename Return, typename Visitor, typename... Args>
		Return Sexpr::visit( Visitor& v, Args... args ) const {
			switch ( type ) {
				#define LLLM_VISITOR( TYPE ) \
					case Sexpr::Type::TYPE: return v.visit( static_cast<TYPE##Ptr>( this ), args... );
				#include "lllm/reader/Sexpr.inc"
			}
		}
		template<typename Return, typename Visitor, typename... Args>
		Return Sexpr::visit( const Visitor& v, Args... args ) const {
			switch ( type ) {
				#define LLLM_VISITOR( TYPE ) \
					case Sexpr::Type::TYPE: return v.visit( static_cast<TYPE##Ptr>( this ), args... );
				#include "lllm/reader/Sexpr.inc"
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
	};
*/
	}; // end namespace reader
}; // end namespace lllm

#endif /* __SEXPR_HPP__ */

