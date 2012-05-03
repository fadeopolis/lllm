
#include "lllm/reader.hpp"
#include "lllm/analyzer.hpp"
#include "lllm/eval.hpp"
#include "lllm/builtins.hpp"
#include "lllm/values/ValueIO.hpp"

#include <cassert>
#include <iostream>

using namespace lllm;
using namespace lllm::analyzer;
using namespace lllm::eval;
using namespace lllm::value;

int main() {
	std::cout << ">>> TESTING EVALUATOR" << std::endl;

	int testsRun = 0, testsPassed = 0;

	GlobalScope globals( &BuiltinScope::builtins );
/*
	GlobalScopePtr scope = &globals;
	EnvPtr         env   = BuiltinScope::builtins.env();

	#define TEST( NAME, REL, INPUT, EXPECTED )  ({									\
		auto str      = (INPUT);													\
		auto actual   = evaluate(													\
							analyze(												\
								read( str ),										\
								scope											\
							),														\
							env												\
						);															\
		auto expected = (EXPECTED);													\
																					\
		if ( (*actual) REL (*expected) ) {											\
			testsPassed++;															\
		} else {																	\
			std::cout << "Test: " NAME " failed: ";									\
			std::cout << "read(" << str << ") == '" << actual << "'";				\
			std::cout << ", should be " #REL " '" << expected << "'" << std::endl;	\
		}																			\
		testsRun++;																	\
	})

	TEST( "", ==, "()",                 nil() );
	TEST( "", ==, "1",                  number( 1 ) );
	TEST( "", ==, "1.5",                number( 1.5 ) );
	TEST( "", !=, "155",                number( 1.5 ) );
	TEST( "", ==, "1 5",                number( 1 ) );
	TEST( "", ==, "'abba",              symbol( "abba" ) );
	TEST( "", ==, "\"abba\"",           string( "abba" ) );
	TEST( "", ==, "(car (cons 1 nil))", number( 1 ) );
*/
	std::cout << ">>> READER PASSED " << testsPassed << " TESTS OUT OF " << testsRun << std::endl;

	#undef TEST

	return 0;
}











