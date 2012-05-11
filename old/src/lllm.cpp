
#include "lllm/reader.hpp"
#include "lllm/analyzer.hpp"
#include "lllm/eval.hpp"
#include "lllm/builtins.hpp"
#include "lllm/vm.hpp"

#include "lllm/reader/SexprIO.hpp"
#include "lllm/analyzer/AstIO.hpp"
#include "lllm/values/ValueIO.hpp"
#include "lllm/util/fail.hpp"

#include <iostream>
#include <iomanip>

using namespace std;
using namespace lllm;
using namespace lllm::eval;
using namespace lllm::util;
using namespace lllm::analyzer;
using namespace lllm::value;

int main() {
	using namespace lllm::reader;

	Reader r = Reader::fromStdin();

	GlobalScope globals( BuiltinScope::builtins() );

	EnvPtr env = BuiltinScope::builtins()->env();

	while ( true ) {
		SexprPtr expr = r.read();

		if ( !expr ) break;

		cout << "READ: " << expr << endl;

		AstPtr ast = analyze( expr, &globals );

		cout << "AST:  " << ast << endl;

		if ( DefinePtr def = dynamic_cast<DefinePtr>( ast ) ) {
			CStr   name = def->var->name;
			AstPtr expr = def->var->value;

			ValuePtr val = evaluate( expr, env );

			globals.addGlobal( reader::SourceLocation("repl"), name, expr );
			env = env->put( name, val );		

			cout << name << " defined to " << val << endl;
		} else {
			ValuePtr val = evaluate( ast, env );
			cout << "===>  " << val << endl;
		}
	}
}














































