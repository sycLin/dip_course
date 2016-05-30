#include <iostream>
#include <vector>

using namespace std;

struct MyStruct {
	// constructor
	MyStruct(int d) {
		data = d;
	}
	// data
	int data;
};

int main(int argc, char* argv[]) {
	cerr << "testing vector of a structure" << endl;
	// declaration
	vector<MyStruct> my_vec;
	my_vec.push_back(MyStruct(123));

	MyStruct hi(456);
	my_vec.push_back(hi);

	cerr << "my_vec size = " << my_vec.size() << endl;
	return 0;
}