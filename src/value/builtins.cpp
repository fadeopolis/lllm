
#include "ValueABI.hpp"
#include "ValueVisitor.hpp"

#include <map>
#include <cassert>
#include <cstring>

using namespace lllm;
using namespace lllm::val;

namespace lllm {
	namespace val {
// ***** constants ***********************************************************************
		static Symbol TRUE("true");

		constexpr ValuePtr True() { return &TRUE; }
// ***** construtors *********************************************************************
		IntPtr    integer( long value )       { return new Int( value ); }
		RealPtr   real( double value )        { return new Real( value ); }
		CharPtr   character( char value )     { return new Char( value ); }
		StringPtr string( const char* value ) { return new String( value ); }
		SymbolPtr symbol( const char* value ) {
			struct cmp_str final {
			   bool operator()(char const *a, char const *b) { 
				return std::strcmp(a, b) < 0;
				}
			};
			typedef std::map<const char*,SymbolPtr, cmp_str> SymbolTable;

			static SymbolTable symbol_table;

			SymbolTable::iterator lb = symbol_table.lower_bound( value );

			if ( lb != symbol_table.end() && !(symbol_table.key_comp()( value, lb->first )) ) {
			    // symbol already exists
				return lb->second;
			} else {
				// the symbol does not exist in the map
				SymbolPtr sym = new Symbol( value );

				// add it to the map using lb as a hint to insert, so it can avoid another lookup
				symbol_table.insert( lb, SymbolTable::value_type( value, sym ) );

				return sym;
			}
		}
		ConsPtr   cons( ValuePtr car, ValuePtr cdr ) { return new Cons( car, cdr ); }
		RefPtr    ref() { return new Ref( nullptr ); }
		RefPtr    ref( ValuePtr v ) { return new Ref( v ); }
// ***** destructuring *******************************************************************
		ValuePtr cadr( ConsPtr cons ) { return car( cast<Cons>( cdr( cons ) ) ); }
		ValuePtr cdar( ConsPtr cons ) { return cdr( cast<Cons>( car( cons ) ) ); }
// ***** predicates **********************************************************************
// *** type checks ***********************************************************************
// *** misc ******************************************************************************
		static inline bool equal( ConsPtr, ConsPtr );

		bool equal( ValuePtr a, ValuePtr b ) {
			if ( a == b ) return true;

			if ( typeOf( a ) != typeOf( b ) ) return false;

			switch ( typeOf( a ) ) {
				case Type::Int:
					switch ( typeOf( b ) ) {
						case Type::Int:  return cast<Int>( a )->value == cast<Int>( b )->value;
						case Type::Real: return cast<Int>( a )->value == cast<Real>( b )->value;
						default:         return false;
					}
				case Type::Real:
					switch ( typeOf( b ) ) {
						case Type::Int:  return cast<Real>( a )->value == cast<Int>( b )->value;
						case Type::Real: return cast<Real>( a )->value == cast<Real>( b )->value;
						default:         return false;
					}
				case Type::Char:
					switch ( typeOf( b ) ) {
						case Type::Char:  return cast<Char>( a )->value == cast<Char>( b )->value;
						default:          return false;
					}
				case Type::String:
					switch ( typeOf( b ) ) {
						case Type::String:  return std::strcmp( cast<String>( a )->value, cast<String>( b )->value ) == 0;
						default:            return false;
					}
				case Type::Symbol:
					return false; // symbols have reference semantics
				case Type::Cons:
					switch ( typeOf( b ) ) {
						case Type::Cons:  return equal( cast<Cons>( a ), cast<Cons>( b ) );
						default:          return false;
					}
				case Type::Nil:
					return false; // there is only one nil
				case Type::Lambda:
					return false; // functions have reference semantics
				case Type::Thunk:
					return false; // thunks have reference semantics
				case Type::Ref:
					return false; // refs have reference semantics
			}
		}

		bool equal( ConsPtr a, ConsPtr b ) {
			return equal( car( a ), car( b ) ) && equal( cdr( a ), cdr( b ) );
		}

		// force instantiation of some constexprs
		void force_constexpr_instantiation() {
			nil();
			True();
			False();

			Cons c( nil(), nil() );
			car( &c );
			cdr( &c );
		}
	} // end namespace val
} // end namespace lllm


