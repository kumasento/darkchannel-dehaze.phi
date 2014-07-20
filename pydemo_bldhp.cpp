#include <iostream>

#include <HazyPixels.h>

using namespace std;

void BldHpFromMatFmt(const char * fl_n){
	hazy_pixels hp(fl_n);
}

extern "C"{
	void pyBldHpFromMatFmt(const char * fl_n){
		BldHpFromMatFmt(fl_n);
	}
}

int main(){
	const char * in_fl_n = "PyLib/tmp_mat_file";

	pyBldHpFromMatFmt(in_fl_n);

	return 0;
}
