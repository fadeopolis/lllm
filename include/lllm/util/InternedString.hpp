#ifndef __STRINGS_HPP__
#define __STRINGS_HPP__ 1

#include "lllm/lllm.hpp"

#include <cstring>

namespace lllm {
	namespace util {
		class InternedString {
			public:
				inline InternedString( CStr str ) : string( intern( str ) ) {}
				inline constexpr InternedString() : string( nullptr ) {}
				inline constexpr InternedString( const InternedString&  str ) : string( str.string ) {}
				inline constexpr InternedString( const InternedString&& str ) : string( str.string ) {}

				inline constexpr operator CStr()  const { return string; }

				inline InternedString& operator=( const InternedString& str ) {
					string = str.string;
					return *this;
				}

				inline constexpr bool operator<( const InternedString& str ) {
					return std::strcmp( string, str.string ) < 0;
				}

				static CStr intern( CStr );
			private:
				CStr string;
		};
	};	

	constexpr bool operator==( const util::InternedString& a, const util::InternedString& b ) { return ((util::CStr)a) == ((util::CStr)b); }
	constexpr bool operator==( util::CStr                  a, const util::InternedString& b ) { return std::strcmp( a, b ) == 0; }
	constexpr bool operator==( const util::InternedString& a, util::CStr                  b ) { return std::strcmp( a, b ) == 0; }
};

#endif /* __STRINGS_HPP__ */

