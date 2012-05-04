
#include "lllm/reader.hpp"
#include "lllm/reader/SexprIO.hpp"

#include <cassert>
#include <iostream>

using namespace lllm;
using namespace lllm::reader;

int main() {

	std::cout << ">>> TESTING READER" << std::endl;

	int testsRun = 0, testsPassed = 0;

	#define TEST( NAME, REL, INPUT, EXPECTED )  ({									\
		auto str      = (INPUT);													\
		auto actual   = read( (NAME), str);													\
		auto expected = (EXPECTED);													\
																					\
		if ( (*actual) REL (*expected) ) {											\
			testsPassed++;															\
		} else {																	\
			std::cout << "Test: " NAME " failed: ";									\
			std::cout << "read( " << str << " ) == '" << actual << "'";				\
			std::cout << ", should be " #REL " '" << expected << "'" << std::endl;	\
		}																			\
		testsRun++;																	\
	})

	TEST( " 1", ==, "()",       nil() );
	TEST( " 2", ==, "1",        number( 1 ) );
	TEST( " 3", ==, "1.5",      number( 1.5 ) );
	TEST( " 4", !=, "155",      number( 1.5 ) );
	TEST( " 5", ==, "1 5",      number( 1 ) );
	TEST( " 6", ==, "abba",     symbol( "abba" ) );
	TEST( " 7", ==, "\"abba\"", string( "abba" ) );
	TEST( " 8", ==, "(1)",      list( number( 1 ) ) );
	TEST( " 9", ==, "(() ())",   list( nil(), nil() ) );
	TEST( "10", ==, "(1 ())",   list( number( 1 ), nil() ) );

	std::cout << ">>> READER PASSED " << testsPassed << " TESTS OUT OF " << testsRun << std::endl;

	#undef TEST

	return 0;
}

