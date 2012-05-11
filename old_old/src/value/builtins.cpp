
#include "values.hpp"
#include "fail.hpp"
#include "Env.hpp"

#include <map>
#include <cassert>
#include <cstring>

using namespace lllm;
using namespace lllm::val;

namespace lllm {
	namespace val {
// ***** constants ***********************************************************************
		const NilPtr   nil   = nil;
		const ValuePtr True  = val::symbol("true");
		const ValuePtr False = nil;
// ***** construtors *********************************************************************
		IntPtr   number( int    value )       { return new Int( value ); }
		IntPtr   number( long   value )       { return new Int( value ); }
		RealPtr  number( float  value )       { return new Real( value ); }
		RealPtr  number( double value )       { return new Real( value ); }
	
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
		ConsPtr   cons( ValuePtr car, ListPtr cdr ) { return new Cons( car, cdr ); }
		ConsPtr   cons( ValuePtr car )              { return cons( car, nil );     }
		LambdaPtr lambda( ListPtr params, ListPtr body, EnvPtr env ) {
			return new Lambda( params, body, env );
		}
		ThunkPtr  thunk( ListPtr body, EnvPtr env ) {
			return new Thunk( body, env );
		}
		RefPtr    ref() { return new Ref( nullptr ); }
		RefPtr    ref( ValuePtr v ) { return new Ref( v ); }
// ***** destructuring *******************************************************************
		ValuePtr car( ListPtr cons )  { return cast<Cons>( cons )->car; }
		ListPtr  cdr( ListPtr cons )  { return cast<Cons>( cons )->cdr; }

		ValuePtr caar( ListPtr cons ) { return cast<Cons>( car( cons ) )->car; }
		ValuePtr cadr( ListPtr cons ) { return car( cdr( cons ) ); }
		ListPtr  cdar( ListPtr cons ) { return cast<Cons>( car( cons ) )->cdr; }
		ListPtr  cddr( ListPtr cons ) { return cdr( cdr( cons ) ); }

		ValuePtr cadar( ListPtr cons ) { return car( cast<Cons>( car( cons ) )->cdr ); }
		ValuePtr caddr( ListPtr cons ) { return car( cdr( cdr( cons ) ) ); }
// ***** predicates **********************************************************************
// *** type checks ***********************************************************************
// *** ref functions *********************************************************************
		ValuePtr get( RefPtr ref ) { return ref->value; }
		void     set( RefPtr ref, ValuePtr val ) { ref->value = val; }
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
		void force_constexpr_instantiation() {
			ValuePtr p;

			p = nil;
			p = True;
			p = False;
		}
	} // end namespace val
} // end namespace lllm


