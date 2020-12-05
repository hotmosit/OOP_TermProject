#include <string>
#include <iostream>

using namespace std;

void main() {

	char buf[76] = "kkkkkk";
	char* ptr = buf;

	for (int i = 0; i < 75; i++) {
		cout << i;
		*(ptr + i) = 'k';
	}
	
	cout << '\n' << buf;
}