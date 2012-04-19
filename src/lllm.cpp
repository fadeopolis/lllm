
//#include "value/Value.hpp"
#include "Value.hpp"
#include "ValueABI.hpp"
#include "ValueIO.hpp"

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
	std::cout << "LLLM says: 'Hi'" << std::endl;

	IntPtr i = val::integer( 5 );
	
	cout << i << endl;
	cout << cons( integer( 0 ), cons( integer( 1 ), nil() ) ) << endl;
	cout << list( 2, list( 3, 4 ) ) << endl;
	cout << list( 5, 5.5f ) << endl;

/*
	Int   i = new IntObj();
	Handle<IntObj>   i2( i );
	Handle<IntObj>   i3 = i;

	Value v;
	Value v_i( i );
	Value v_n = val::nil;

	cout << (v.type()   == Type::Nil) << endl;
	cout << (v_i.type() == Type::Int) << endl;

//	lllm::Value v;
//
//	std::cout << "E " << sizeof( E ) << std::endl;
//	std::cout << "U " << sizeof( U ) << std::endl;
//	std::cout << "S " << sizeof( S ) << std::endl;
*/
	return 0;
}

