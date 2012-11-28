
#include "lllm/Reader.hpp"
#include "lllm/Analyzer.hpp"
#include "lllm/Evaluator.hpp"
#include "lllm/GlobalScope.hpp"
#include "lllm/sexpr/SexprIO.hpp"
#include "lllm/ast/AstIO.hpp"
#include "lllm/value/ValueIO.hpp"
#include "lllm/util/util_io.hpp"

#include <cassert>
#include <iostream>

using namespace lllm;
using namespace lllm::value;
using namespace lllm::util;

int main() {
	std::cout << ">>> TESTING EVALUATOR" << std::endl;

	Evaluator::setJittingThreshold( 999999999 );

	int testsRun = 0, testsPassed = 0;

	GlobalScope scope;

	#define TEST( NAME, REL, INPUT, EXPECTED )  ({									\
		auto name     = (NAME);														\
		auto str      = (INPUT);													\
		auto actual   = Evaluator::evaluate(										\
							Analyzer::analyze(										\
								Reader::read( str ),								\
								&scope												\
							),														\
							&scope													\
						);															\
		auto expected = (EXPECTED);													\
																					\
		if ( (*actual) REL (*expected) ) {											\
			testsPassed++;															\
		} else {																	\
			std::cout << "Test: " << name << " failed: ";							\
			std::cout << str << " == '" << actual << "'";							\
			std::cout << ", should be " #REL " '" << expected << "'" << std::endl;	\
		}																			\
		testsRun++;																	\
	})

// (define sum (lambda sum (a b) (if (= a 0) b (sum (- a 1) (+ 1 b)))))

	TEST( " 1",            ==, "()",                 nil );
	TEST( " 2",            ==, "1",                  number( 1 ) );
	TEST( " 3",            ==, "1.5",                number( 1.5 ) );
	TEST( " 4",            !=, "155",                number( 1.5 ) );
	TEST( " 5",            ==, "1 5",                number( 1 ) );
	TEST( " 6",            ==, "'abba",              symbol( "abba" ) );
	TEST( " 7",            ==, "\"abba\"",           string( "abba" ) );
	TEST( " 8",            ==, "(car (cons 1 nil))", number( 1 ) );
	TEST( " 9",            ==, "(+ 4.5 5)",          number( 9.5 ) );
	TEST( "10",            ==, "(define a 5)",       number( 5 ) );
	TEST( "const fn",      ==, "((lambda a () 5))",  number( 5 ) );
	TEST( "const lambda",  ==, "(((lambda const (x) (lambda (y) x)) 'x) 'y)",  symbol("x") );
	TEST( "recursion",     ==, "((lambda sum (a b) (if (= a 0) b (sum (- a 1) (+ 1 b)))) 5 5)", number( 10 ) );
	TEST( "fib",           ==, "((lambda fib (n) (if (< n 2) n (+ (fib (- n 2)) (fib (- n 1))))) 6)", number(8) );
	TEST( "tail_fib",      ==, "((lambda tail_fib (n result next) (if (= n 0) result (tail_fib (- n 1) next (+ result next)))) 8 0 1)", number(21) );

	TEST("xxx", ==, "((lambda sum (a b) (if (<= a 0) b (let (a (- a 1)) (b (+ 1 b)) (if (<= a 0) b (sum (- a 1) (+ 1 b)))))) 5 6)", number(11) );

	std::cout << ">>> EVALUATOR PASSED " << testsPassed << " TESTS OUT OF " << testsRun << std::endl;

	#undef TEST

	return 0;
}































