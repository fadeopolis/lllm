
#include "lllm/analyzer.hpp"
#include "lllm/analyzer/AstIO.hpp"

#include <cassert>
#include <iostream>

using namespace lllm;
using namespace lllm::analyzer;

int main() {
	std::cout << ">>> TESTING ANALYZER" << std::endl;

	int testsRun = 0, testsPassed = 0;
/*
	#define TEST( NAME, REL, STR, OBJ )  ({												\
		auto str = (STR);																\
		auto obj = read(OBJ);															\
																						\
		if ( read( str ) REL obj ) {													\
			std::cout << "Test: " NAME " failed: read(" STR ") == '" << read( str );	\
			std::cout << "' should equal '" << obj << "'" << std::endl;					\
		} else {																		\
			testsPassed++;																\
		}																				\
		testsRun++;																		\
	})

	TEST( "", ==, "1",        number( 1 ) );
	TEST( "", ==, "1.5",      number( 1.5 ) );
	TEST( "", !=, "155",      number( 1.5 ) );
	TEST( "", ==, "1 5",      number( 1 ) );
	TEST( "", ==, "abba",     symbol( "abba" ) );
	TEST( "", ==, "\"abba\"", string( "abba" ) );
	TEST( "", ==, "()",       nil() );
	TEST( "", ==, "(1)",      list( number( 1 ) ) );
	TEST( "", ==, "(()())",   list( nil(), nil() ) );
	TEST( "", ==, "(1 ())",   list( number( 1 ), nil() ) );
*/
	std::cout << ">>> ANALYZER PASSED " << testsPassed << " TESTS OUT OF " << testsRun << std::endl;

	#undef TEST

	return 0;
}

