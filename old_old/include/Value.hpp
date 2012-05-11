#ifndef __TEST_HPP__
#define __TEST_HPP__ 1

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

 	typedef const class Value*  ValuePtr;
	typedef const class List*   ListPtr;
	typedef const class Nil*    NilPtr;
	typedef const class Cons*   ConsPtr;
	typedef const class Number* NumberPtr;
	typedef const class Int*    IntPtr;
	typedef const class Real*   RealPtr;
	typedef const class Char*   CharPtr;
	typedef const class String* StringPtr;
	typedef const class Symbol* SymbolPtr;
	typedef const class Lambda* LambdaPtr;
	typedef const class Thunk*  ThunkPtr;
	typedef const class Ref*    RefPtr;
	typedef const class Env*    EnvPtr;
	typedef const char*         CStr;		

	namespace val {
		// *** constants
		extern const NilPtr   nil;
		extern const ValuePtr True;
		extern const ValuePtr False;
		// *** construtors
		extern IntPtr    number( int    );
		extern IntPtr    number( long   );
		extern RealPtr   number( float  );
		extern RealPtr   number( double );
		extern CharPtr   character( char );
		extern StringPtr string( const char* );
		extern SymbolPtr symbol( const char* );
		extern ConsPtr   cons( ValuePtr car, ListPtr cdr );
		extern ConsPtr   cons( ValuePtr car );
		extern LambdaPtr lambda( ListPtr params, ListPtr body, EnvPtr env );
		extern ThunkPtr  thunk( ListPtr body, EnvPtr env );
		extern RefPtr    ref();
		extern RefPtr    ref( ValuePtr v );
		// *** destructuring
		extern ValuePtr car( ListPtr cons );
		extern ListPtr  cdr( ListPtr cons );

		extern ValuePtr caar( ListPtr cons );
		extern ValuePtr cadr( ListPtr cons );
		extern ListPtr  cdar( ListPtr cons );
		extern ListPtr  cddr( ListPtr cons );

		extern ValuePtr cadar( ListPtr cons );
		extern ValuePtr caddr( ListPtr cons );

		// *** predicates
		// ** misc
		extern bool equal( ValuePtr, ValuePtr );
		
		constexpr Type typeOf( ValuePtr );

		// *** list functions
		extern unsigned length( ListPtr );

		// *** ref functions
		extern ValuePtr get( RefPtr ref );
		extern void     set( RefPtr ref, ValuePtr val );
	}

	class Value {
		private:
			inline constexpr Value( Type type ) : type( type ) {}

			const Type type;

		friend class List;
		friend class Number;
		friend class Char;
		friend class String;
		friend class Symbol;
		friend class Lambda;
		friend class Thunk;
		friend class Ref;
		friend constexpr Type val::typeOf( ValuePtr );
	};

	class List : public Value {
		private:
			inline constexpr List( Type type ) : Value( type ) {}

		friend class Nil;
		friend class Cons;
	};
	class Nil : public List {
		private:
			Nil()                        = delete;
			Nil( const Nil& )            = delete;
			Nil& operator=( const Nil& ) = delete;
	};
	class Cons : public List {
		public:
			static constexpr Type TYPE = Type::Cons;

			const ValuePtr car;
			const ListPtr  cdr;
		private:
			Cons( ValuePtr car, ListPtr cdr );

		friend ConsPtr val::cons( ValuePtr, ListPtr );
	};

	class Number : public Value {
		private:
			inline constexpr Number( Type type ) : Value( type ) {}

		friend class Int;
		friend class Real;
	};
	class Int : public Number {
		public:
			static constexpr Type TYPE = Type::Int;

			const long value;	
		private:
			Int( long value );

		friend IntPtr val::number( int  );
		friend IntPtr val::number( long );
	};
	class Real : public Number {
		public:
			static constexpr Type TYPE = Type::Real;

			const double value;
		private:
			Real( double value );
	
		friend RealPtr val::number( float  );
		friend RealPtr val::number( double );
	};

	class Char : public Value {
		public:
			static constexpr Type TYPE = Type::Char;

			const char value;
		private:
			Char( char value );

		friend CharPtr val::character( char );
	};
	class String : public Value {
		public:
			static constexpr Type TYPE = Type::String;
	
			const char* const value;
		private:
			String( const char* value );

		friend StringPtr val::string( const char* );
	};
	class Symbol : public Value {
		public:
			static constexpr Type TYPE = Type::Symbol;

			const char* const value;
		private:
			Symbol( const char* value );

		friend SymbolPtr val::symbol( const char* );
	};
	class Lambda : public Value {
		public:
			static constexpr Type TYPE = Type::Lambda;

			const ListPtr parameters;    // the names of the functions parameters
			const ListPtr body;          // the AST of the functions body
			const EnvPtr  env;           // the environment the function was defined in
		private:
			Lambda( ListPtr params, ListPtr body, EnvPtr env );

		friend LambdaPtr val::lambda( ListPtr params, ListPtr body, EnvPtr env );
	};
	class Thunk : public Value {
		public:
			static constexpr Type TYPE = Type::Thunk;

			const ListPtr body;          // the AST of the functions body
			const EnvPtr  env;           // the environment the function was defined in
		private:
			Thunk( ListPtr body, EnvPtr env );
			
		friend ThunkPtr val::thunk( ListPtr body, EnvPtr env );
	};
	class Ref : public Value {
		public:
			static constexpr Type TYPE = Type::Ref;
		private:
			Ref();
			Ref( ValuePtr );

			mutable ValuePtr value;
		
		friend RefPtr   val::ref();
		friend RefPtr   val::ref( ValuePtr );
		friend ValuePtr val::get( RefPtr );
		friend void     val::set( RefPtr, ValuePtr );
	};

	// checked down-cast
	template<typename T> const T* cast( ValuePtr v );
	template<typename T> const T* castOrNil( ValuePtr v );

	template<> NilPtr    cast<Nil>   ( ValuePtr );
	template<> ConsPtr   cast<Cons>  ( ValuePtr );
	template<> IntPtr    cast<Int>   ( ValuePtr );
	template<> RealPtr   cast<Real>  ( ValuePtr );
	template<> CharPtr   cast<Char>  ( ValuePtr );
	template<> StringPtr cast<String>( ValuePtr );
	template<> SymbolPtr cast<Symbol>( ValuePtr );
	template<> LambdaPtr cast<Lambda>( ValuePtr );
	template<> ThunkPtr  cast<Thunk> ( ValuePtr );
	template<> RefPtr    cast<Ref>   ( ValuePtr );

	template<> ConsPtr   castOrNil<Cons>  ( ValuePtr );
	template<> IntPtr    castOrNil<Int>   ( ValuePtr );
	template<> RealPtr   castOrNil<Real>  ( ValuePtr );
	template<> CharPtr   castOrNil<Char>  ( ValuePtr );
	template<> StringPtr castOrNil<String>( ValuePtr );
	template<> SymbolPtr castOrNil<Symbol>( ValuePtr );
	template<> LambdaPtr castOrNil<Lambda>( ValuePtr );
	template<> ThunkPtr  castOrNil<Thunk> ( ValuePtr );
	template<> RefPtr    castOrNil<Ref>   ( ValuePtr );

	namespace val {
		constexpr Type typeOf( ValuePtr v ) {
			return v ? v->type : Type::Nil;
		}
	}
}

#endif /* __TEST_HPP__ */

