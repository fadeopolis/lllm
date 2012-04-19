
//#include "value/Value.hpp"
#include "Value.hpp"
#include "ValueABI.hpp"
#include "ValueIO.hpp"
#include "Reader.hpp"

#include <cassert>
#include <iostream>
#include <string>

using namespace std;
using namespace lllm;
using namespace lllm::val;

static constexpr double D = 5.5;

static constexpr Int I( 5 );
static constexpr Cons c( val::nil(), val::nil() );

static constexpr long     l   = I.value;
static constexpr ValuePtr car = val::car( &c );

int main() {
	cout << "LLLM says: 'Hi'" << endl << endl;

	IntPtr i = val::integer( 5 );
	
	cout << i << endl;
	cout << cons( integer( 0 ), cons( integer( 1 ), nil() ) ) << endl;
	cout << "'" << nil() << "'" << endl;
	
	cout << endl << ">>> TEST EQUALITY" << endl;
	cout << std::boolalpha;

	#define EQ( A, B ) (void) ({ cout << #A " == " #B " is " << equal( (A), (B) ) << endl; nullptr; })

	EQ( nil(),        nil() );
	EQ( integer( 5 ), nil() );
	EQ( nil(),        integer( 5 ) );
	EQ( integer( 5 ), integer( 5 ) );
	EQ( cons( nil(), nil() ), cons( nil(), True() ) );

	cout << endl << "LLLM says: 'Bye'" << endl;

	return 0;
}

