
#include "lllm/lllm.hpp"
#include "lllm/ast.hpp"
#include "lllm/sexpr.hpp"
#include "lllm/GlobalScope.hpp"
#include "lllm/Reader.hpp"
#include "lllm/Analyzer.hpp"
#include "lllm/Evaluator.hpp"
#include "lllm/Jit.hpp"
#include "lllm/Builtins.hpp"

#include "lllm/sexpr/SexprIO.hpp"
#include "lllm/ast/AstIO.hpp"
#include "lllm/value/ValueIO.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>

#include <map>

using namespace lllm;
using namespace lllm::util;

CStr argsString( int argc, char** argv ) {
	argc--;
	argv++;

	size_t len = 0;

	for ( int i = 0; i < argc; i++ ) len += strlen( argv[i] );

	char* cs = (char*) GC_MALLOC_ATOMIC( len + 1 );
	cs[0] = 0;

	for ( int i = 0; i < argc; i++ ) {
		cs = strcat( cs, argv[i] );
	}

	return cs;
}

int main( int argc, char** argv ) {
	std::cout << "Starting LLLM REPL" << std::endl;
	std::cout << ">> " << std::flush;

	Evaluator::setJittingThreshold( 5 );

	Jit::setInliningThreshold( 10 );

	GlobalScope scope;

	Reader r = argc <= 1 ? Reader::fromStdin() : Reader::fromString( argsString( argc, argv ) );

	while ( true ) {
		std::cout << ">> " << std::flush;

		sexpr::SexprPtr exp = r.read();

		if ( exp == nullptr ) break;

		std::cout << "READ:  " << std::flush << exp << std::endl;

		ast::AstPtr     ast = Analyzer::analyze( exp, &scope );

		std::cout << "AST:   " << std::flush << ast << std::endl;

		if ( ast::DefinePtr def = dynamic_cast<ast::DefinePtr>( ast ) ) {
			value::ValuePtr val = Evaluator::evaluate( def->expr, &scope );

			scope.add( def->location, def->name, ast, val );

			std::cout << "DEFINED " << def->name << " TO " << val << std::endl;
		} else {
			value::ValuePtr val = Evaluator::evaluate( ast, &scope );

			if ( val == Builtins::CLEAR_MARK ) {
				std::cout << "\033[2J\033[1;1H" << "LLLM REPL\n" << std::flush;
			} else {
				std::cout << "VALUE: " << val << " :: " << value::typeOf( val ) << std::endl;
			}
		}
	}

	std::cout << "LLLM REPL, over and out" << std::endl;
}
