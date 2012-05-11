
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

	Reader    r = argc <= 1 ? Reader::fromStdin() : Reader::fromString( argsString( argc-1, argv+1) );
	Analyzer  a;
	Evaluator e;
//	Jit       j;

	GlobalScope scope;

	while ( true ) {
		sexpr::SexprPtr exp = r.read();

		if ( exp == nullptr ) break;

		std::cout << "READ:  " << exp << std::endl;

		ast::AstPtr     ast = a.analyze( exp, &scope );

		std::cout << "AST:   " << ast << std::endl;

		if ( ast::DefinePtr def = dynamic_cast<ast::DefinePtr>( ast ) ) {
			value::ValuePtr val = e.evaluate( def->expr, &scope );	

			scope.add( def->location, def->name, val );

			std::cout << "DEFINED " << def->name << " TO " << val << std::endl;			
		} else {
			value::ValuePtr val = e.evaluate( ast, &scope );

			std::cout << "VALUE: " << val << " :: " << value::typeOf( val ) << std::endl;
		}
	}

	std::cout << "LLLM REPL, over and out" << std::endl;		
}

