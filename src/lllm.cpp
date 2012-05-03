
#include "lllm/analyzer.hpp"
#include "lllm/reader.hpp"
#include "lllm/vm.hpp"

#include "lllm/reader/SexprIO.hpp"
#include "lllm/analyzer/AstIO.hpp"
#include "lllm/util/fail.hpp"

#include <iostream>
#include <iomanip>

using namespace std;
using namespace lllm;
using namespace lllm::util;
using namespace lllm::analyzer;

int main() {
	using namespace lllm::reader;

	VM* vm = VM::make();

	#define ECHO( STR ) ({ SexprPtr ptr = read( (STR) ); cout << (STR) << "\tbecomes " << flush << analyze( ptr, vm->scope() ) << endl; ; })

	ECHO( "(if 1 2 3)                              " );
	ECHO( "'(if 1 2 3)                             " );
	ECHO( "(define name 5)                         " );
	ECHO( "(let ((a 1)) 1)                         " );
	ECHO( "(let ((a 1)(b 2)) b)                    " );
	ECHO( "()                                      " );
	ECHO( "(lambda (a) a)                          " );
	ECHO( "(list 1 2 3)                            " );
	ECHO( "(let ((a (lambda (a b c) a))) (a 1 2 3))" );
}














































