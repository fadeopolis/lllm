
/*
#include "Value.hpp"
#include "ValueIO.hpp"
#include "read.hpp"
#include "eval.hpp"
#include "fail.hpp"
*/

#include "reader.hpp"

#include <cassert>
#include <iostream>
#include <string>

namespace lllm {
	namespace val  {}
	namespace util {}
}

using namespace std;
using namespace lllm;
using namespace lllm::val;

static int testReader();

int main() {
	cout << "LLLM says: 'Hi'" << endl << endl;
/*
	ValuePtr i = val::number( 5 );
	
	cout << i << endl;
	cout << cons( number( 0 ), cons( number( 1 ), nil() ) ) << endl;
	cout << "'" << nil() << "'" << endl;
*/
/*
	cout << endl << ">>> TEST EQUALITY" << endl;
	cout << std::boolalpha;

	#define EQ( A, B ) (void) ({ cout << #A " == " #B " is " << equal( (A), (B) ) << endl; nullptr; })

	EQ( nil(),       nil() );
	EQ( number( 5 ), nil() );
	EQ( nil(),       number( 5 ) );
	EQ( number( 5 ), number( 5 ) );
	EQ( cons( nil(), nil() ), cons( nil(), True ) );

	cout << std::noboolalpha;
*/

	cout << endl << ">>> TEST READER" << endl;

	#define READ( A ) (void) ({ cout << "read(" A ") is " << read( A ) << endl; nullptr; })

	READ("()");
	READ("(lambda () a)");

/*
	cout << endl << ">>> TEST EVAL" << endl;

	#define EVAL( A ) (void) ({ cout << "eval(" A ") is " << eval( read(A), builtins() ) << endl; nullptr; })

	EVAL("(do nil (define a 6) 5 a)");
	EVAL("(let ((a 5)) a)");
	EVAL("1");
	EVAL("\"true\"");
	EVAL("true");
	EVAL("false");
	EVAL("nil");
	EVAL("\\c");
	EVAL("'(if true 0 1)");
	EVAL("'(if true 0 1)");
	EVAL("(if true  0 1)");
	EVAL("(if false 0 1)");
	EVAL("(if true (if false 1 0) 1)");
	EVAL("(lambda () 'a)");
	EVAL("((lambda () 5))");
*/
	cout << endl << "LLLM says: 'Bye'" << endl;
	return 0;
}

static int runReaderTest( const char* str, const ParseTree& tree );
static int runReaderTest( const char* str, const ParseTree* tree );

int testReader() {
	return 0;
}

int runReaderTest( const char* str, const ParseTree& tree ) {
	return 0;
}
int runReaderTest( const char* str, const ParseTree* tree ) {
	return runReaderTest( str, *tree );
}

