
#include <iostream>
#include <sstream>

using namespace std;

void foo( istream& in ) {
	int i = in.get();
	cout << i << endl;
}

int main() {
	stringstream str("");	

	foo( str );
}


