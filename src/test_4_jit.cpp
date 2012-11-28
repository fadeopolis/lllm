
#include "lllm/Reader.hpp"
#include "lllm/Analyzer.hpp"
#include "lllm/Evaluator.hpp"
#include "lllm/Jit.hpp"
#include "lllm/GlobalScope.hpp"
#include "lllm/value/ValueIO.hpp"
#include "lllm/ast/AstIO.hpp"
#include "lllm/util/util_io.hpp"

#include <cassert>
#include <iostream>

using namespace lllm;
using namespace lllm::value;
using namespace lllm::util;

long fib( long n, long result = 0, long next = 1 ) {
	if ( n == 0 ) {
		return result;
	} else {
		return fib( n - 1, next, result + next );
	}
}

int main() {
	GC_init();

	std::cout << ">>> TESTING JIT" << std::endl;

	Evaluator::setJittingThreshold( 0 );

	Jit::setInliningThreshold( 10 );

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
			std::cout << "Test: " << name << " passed: ";							\
			std::cout << str << " == " << typeOf( actual ) << " '" << actual << "'";\
			std::cout << std::endl;													\
			testsPassed++;															\
		} else {																	\
			std::cout << "Test: " << name << " failed: ";							\
			std::cout << str << " == " << typeOf( actual ) << " '" << actual << "'";\
			std::cout << ", should be ";											\
			std::cout << #REL " " << typeOf( expected ) << " '" << expected << "'";	\
			std::cout << std::endl;													\
		}																			\
		testsRun++;																	\
	})

	#define GLOBAL( NAME, BODY ) ({ 						\
		auto sexpr = Reader::read( BODY );					\
		auto ast   = Analyzer::analyze( sexpr, &scope );	\
		auto val   = Evaluator::evaluate( ast, &scope );	\
															\
		scope.add( 											\
			SourceLocation("*global*"), 					\
			NAME,											\
			ast,											\
			val												\
		);													\
	})

	GLOBAL( "id",       "(lambda id    (x)   x)" );
	GLOBAL( "sum",      "(lambda sum   (a b) (if (<= a 0) b (sum (- a 1) (+ 1 b)))))" );
	GLOBAL( "const",    "(lambda const (x)   (lambda (y) x))" );
	GLOBAL( "fib",      "(lambda fib (n) (if (< n 2) n (+ (fib (- n 2)) (fib (- n 1)))))" );
	GLOBAL( "tail_fib", "(lambda tail_fib (n result next) (if (= n 0) result (tail_fib (- n 1) next (+ result next))))" );
	GLOBAL( "rec",      "(lambda rec (a) (if (> a 0) (rec (- a 1)) 5.5)))" );
	GLOBAL( "mul",      "(lambda A (a b) (* a b))" );
	GLOBAL( "apply2",   "(lambda B (fn a b) (fn a b))" );
	GLOBAL( "!",        "(lambda ! (n) (if (<= n 0) 1 (* n (! (- n 1)))))" );
//	TEST( "", ==, "((lambda x (a b) (sum2 a b)) 5 5)", False );

// (define id (lambda id    (x)   x))

	TEST( " 0",             ==, "((lambda (a b) (+ a b)) 1 2)", number(3) );

	TEST( " 1",             ==, "(+ 1 1)",                                           number(2)        );
	TEST( " 2",             ==, "(- 2 1)",                                           number(1)        );
	TEST( " 3",             ==, "(> 2 1)",                                           True()           );
	TEST( " 4",             ==, "(< 2 1)",                                           False            );
	TEST( " 5",             ==, "((lambda id   (x) x) 'x)",                          symbol("x")      );
	TEST( " 6",             ==, "((lambda if1  (x) (if x 'x 'y)) 1)",                symbol("x")      );
	TEST( " 7",             ==, "((lambda if2  (x) (if x 'x 'y)) nil)",              symbol("y")      );
	TEST( " 8",             ==, "((lambda let1 (x) (let (a 5) a)) 7)",               number(5)        );
	TEST( "sum-1",          ==, "(sum 99999 1)",                                     number(100000)   );
	TEST( "sum-2",          ==, "(sum 9999999 1)",                                   number(10000000) );
	TEST( "const fn",       ==, "((lambda a () 5))",                                 number( 5 )      );
	TEST( "const lambda",   ==, "((const 'x) 'y)",                                   symbol("x")      );
	TEST( "fib",            ==, "(fib 8)",                                           number(fib(8))   );
	TEST( "closures",       ==, "(apply2 mul 4 3)",                                  number(12)       );
	TEST( "tail_fib",       ==, "(tail_fib 8 0 1)",                                  number(fib(8))   );
	TEST( "factorial",      ==, "(! 6)",                                             number(720)      );
//	TEST( "xxx", !=, "(lamba a (x) ", nil );

	std::cout << ">>> JIT PASSED " << testsPassed << " TESTS OUT OF " << testsRun << std::endl;

	#undef TEST

	return 0;
}














