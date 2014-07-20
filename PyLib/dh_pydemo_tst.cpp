#include <iostream>
#include <fstream>

using namespace std;

void fileIO(char * fl_n){
	ofstream o_fl(fl_n);
	o_fl << "hello world";
}
extern "C"{
	void wrapped_fileIO(char * fl_n){
		fileIO(fl_n);
	}
}
