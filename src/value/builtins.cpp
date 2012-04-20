
#include "Value_internals.hpp"
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

		constexpr ValuePtr True  = &TRUE;
		constexpr ValuePtr False = nil();
// ***** construtors *********************************************************************
		ValuePtr number( int    value )      { return new Int( value ); }
		ValuePtr number( long   value )      { return new Int( value ); }
		ValuePtr number( double value )      { return new Real( value ); }
		ValuePtr character( char value )     { return new Char( value ); }
		ValuePtr string( const char* value ) { return new String( value ); }
		ValuePtr symbol( const char* value ) {
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
		ValuePtr cons( ValuePtr car, ValuePtr cdr ) { return new Cons( car, cdr ); }
		ValuePtr ref() { return new Ref( nullptr ); }
		ValuePtr ref( ValuePtr v ) { return new Ref( v ); }
// ***** destructuring *******************************************************************
		ValuePtr car( ValuePtr cons )  { return cast<Cons>( cons )->car; }
		ValuePtr cdr( ValuePtr cons )  { return cast<Cons>( cons )->cdr; }

		ValuePtr cadr( ValuePtr cons ) { return car( cdr( cons ) ); }
		ValuePtr cdar( ValuePtr cons ) { return cdr( car( cons ) ); }

		ValuePtr cadar( ValuePtr cons ) { return car( cdr( car( cons ) ) ); }
		ValuePtr caddr( ValuePtr cons ) { return car( cdr( cdr( cons ) ) ); }
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
						case Type::Int:  return static_cast<IntPtr>( a )->value == static_cast<IntPtr>( b )->value;
						case Type::Real: return static_cast<IntPtr>( a )->value == static_cast<RealPtr>( b )->value;
						default:         return false;
					}
				case Type::Real:
					switch ( typeOf( b ) ) {
						case Type::Int:  return static_cast<RealPtr>( a )->value == static_cast<IntPtr>( b )->value;
						case Type::Real: return static_cast<RealPtr>( a )->value == static_cast<RealPtr>( b )->value;
						default:         return false;
					}
				case Type::Char:
					switch ( typeOf( b ) ) {
						case Type::Char:  return static_cast<CharPtr>( a )->value == static_cast<CharPtr>( b )->value;
						default:          return false;
					}
				case Type::String:
					switch ( typeOf( b ) ) {
						case Type::String:  return std::strcmp( static_cast<StringPtr>( a )->value, static_cast<StringPtr>( b )->value ) == 0;
						default:            return false;
					}
				case Type::Symbol:
					return false; // symbols have reference semantics
				case Type::Cons:
					switch ( typeOf( b ) ) {
						case Type::Cons:  return equal( static_cast<ConsPtr>( a ), static_cast<ConsPtr>( b ) );
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
/*		void force_constexpr_instantiation() {
			nil();
			True();
			False();

			Cons c( nil(), nil() );
			car( &c );
			cdr( &c );
		}
*/	} // end namespace val
} // end namespace lllm


