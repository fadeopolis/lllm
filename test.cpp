
#include "lllm/util/Ptr.tpp"

#include <iostream>

using namespace std;
using namespace lllm;

struct A {
	A() : i( 0 ) {}
	A( int i_ ) : i( i_ ) {}

	int i;

	operator const void*() const {
		return this;
	}
};

typedef Ptr<A> APtr;

int main() {
	APtr a1 = new A();
	APtr a2 = (nullptr);

	cout << a1->i << endl;
	cout << a2 << endl;
}
