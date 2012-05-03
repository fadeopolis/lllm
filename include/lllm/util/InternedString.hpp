#ifndef __INTERNED_STRING_HPP__
#define __INTERNED_STRING_HPP__

namespace lllm {
	typedef const char* CStr;

	namespace util {
		class InternedString {
			public:
				inline InternedString( CStr str ) : string( intern( str ) ) {}
				inline constexpr InternedString() : string( nullptr ) {}
				inline constexpr InternedString( const InternedString&  str ) : string( str.string ) {}
				inline constexpr InternedString( const InternedString&& str ) : string( str.string ) {}

				inline constexpr operator CStr() const { return string; }
			
				inline InternedString& operator=( const InternedString& str ) {
					string = str.string;
					return *this;
				}

				inline constexpr bool operator==( const InternedString& that ) const {
					return string == that.string;
				}
			private:
				static CStr intern( CStr );

				CStr string;
		};
	}; // end namespace util
}; // end namespace lllm

#endif /* __INTERNED_STRING_HPP__ */


