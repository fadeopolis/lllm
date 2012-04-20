
//#include "value/Value.hpp"
#include "Value.hpp"
#include "Value_internals.hpp"
#include "ValueIO.hpp"
#include "read.hpp"
#include "eval.hpp"
#include "fail.hpp"

#include <cassert>
#include <iostream>
#include <string>

using namespace std;
using namespace lllm;
using namespace lllm::val;

int main() {
	cout << "LLLM says: 'Hi'" << endl << endl;
/*
	ValuePtr i = val::number( 5 );
	
	cout << i << endl;
	cout << cons( number( 0 ), cons( number( 1 ), nil() ) ) << endl;
	cout << "'" << nil() << "'" << endl;
	
	cout << endl << ">>> TEST EQUALITY" << endl;
	cout << std::boolalpha;

	#define EQ( A, B ) (void) ({ cout << #A " == " #B " is " << equal( (A), (B) ) << endl; nullptr; })

	EQ( nil(),       nil() );
	EQ( number( 5 ), nil() );
	EQ( nil(),       number( 5 ) );
	EQ( number( 5 ), number( 5 ) );
	EQ( cons( nil(), nil() ), cons( nil(), True ) );

	cout << endl << ">>> TEST EVAL" << endl;
	cout << std::noboolalpha;
*/
	#define EVAL( A )     (void) ({ cout << "eval(" #A ") is " << eval( (A), builtins() ) << endl; nullptr; })
	#define EVAL_STR( A ) (void) ({ cout << "eval(" A ") is " << eval( read(A), builtins() ) << endl; nullptr; })

	ValuePtr l = read("(if true (if false 1 0) 1)");

	cout << l << endl;

	cout << val::car( l ) << endl;
	cout << val::cadr( l ) << endl;
	cout << val::caddr( l ) << endl;
	cout << endl;

	EVAL( number(5) );
	EVAL_STR("'(if true 0 1)");
	EVAL_STR("(if true  0 1)");
	EVAL_STR("(if false 0 1)");
	EVAL_STR("(if true (if false 1 0) 1)");

	cout << endl << "LLLM says: 'Bye'" << endl;

	return 0;
}

