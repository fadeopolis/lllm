
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

ast::AstPtr mkAST( CStr str, Analyzer& analyzer, ScopePtr<ast::VariablePtr> scope ) {
	return analyzer.analyze( Reader::read( str ), scope );
}

ValuePtr eval( CStr str, GlobalScope& scope ) {
	return Evaluator::evaluate( Analyzer::analyze( Reader::read( str ), &scope ), &scope );
}

int main() {
	GC_init();

	std::cout << ">>> TESTING JIT" << std::endl;

	Evaluator::setJittingThreshold( 0 );

	int testsRun = 0, testsPassed = 0;

	GlobalScope scope;

	#define TEST( NAME, REL, INPUT, EXPECTED )  ({									\
		auto name     = (NAME);														\
/*																					\
		std::cout << ">>>\tRUNNING TEST " << name << std::endl;						\
*/																					\
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
			std::cout << "read(" << str << ") == '" << actual << "'";				\
			std::cout << ", should be " #REL " '" << expected << "'" << std::endl;	\
		}																			\
		testsRun++;																	\
	})

	#define GLOBAL( NAME, BODY ) ({ 		\
		scope.add( 							\
			SourceLocation("*global*"), 	\
			NAME,							\
			Evaluator::evaluate(			\
				Analyzer::analyze(			\
					Reader::read( BODY ),	\
					&scope 					\
				), 							\
				&scope						\
			 )								\
		);									\
	})

	GLOBAL( "id",    "(lambda id    (x)   x)" );
	GLOBAL( "sum",   "(lambda sum   (a b) (if (= a 0) b (sum (- a 1) (+ 1 b)))))" );
	GLOBAL( "const", "(lambda const (x)   (lambda (y) x))" );

	TEST( " 1",           ==, "(+ 1 1)",                                           number(2)     );
	TEST( " 2",           ==, "((lambda id   (x) x) 'x)",                          symbol("x")   );
	TEST( " 3",           ==, "((lambda if1  (x) (if x 'x 'y)) 1)",                symbol("x")   );
	TEST( " 4",           ==, "((lambda if2  (x) (if x 'x 'y)) nil)",              symbol("y")   );
	TEST( " 5",           ==, "((lambda let1 (x) (let ((a 5)) a)) 7)",             number(5)     );
	TEST( " 6",           ==, "(sum 1000 7)",                                     number(1007) );
	TEST( "const fn",     ==, "((lambda a () 5))",  number( 5 ) );
	TEST( "const lambda", ==, "((const 'x) 'y)",  symbol("x") );
	TEST( " 7",           ==, "(((lambda const (x) (lambda inner (y) x)) 'x) 'y)", symbol("x")   );

	std::cout << ">>> JIT PASSED " << testsPassed << " TESTS OUT OF " << testsRun << std::endl;

	#undef TEST

	return 0;
}











