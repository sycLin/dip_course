#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
	int max = 95;
	while(1) {
		int input;
		cin >> input;
		int trimmed = (input < 0) ? 0 : (input >= max) ? (max-1) : input;
		cout << "trimmed: " << trimmed << endl;
	}
	return 0;
}