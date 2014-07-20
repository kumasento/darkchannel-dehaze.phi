#include "../MatFmtInterfaces.h"

typedef unsigned char byte;

void MatFmtLnW(const char *in_fl_n, const char *out_fl_n)
{
	int h, w;
	if(MatFmt::MatFmtHWLdr(h, w, in_fl_n))
		printf("HWLdr OK\n");
	int sz = h * w;

	byte * R_arr = (byte*) malloc(sizeof(byte) * sz);
	byte * G_arr = (byte*) malloc(sizeof(byte) * sz);
	byte * B_arr = (byte*) malloc(sizeof(byte) * sz);
	
	if(MatFmt::MatFmtRGBLdr<byte>(R_arr, G_arr, B_arr, h, w, in_fl_n))
		printf("Ldr OK\n");
	if(MatFmt::MatFmtRGBWtr<byte>(R_arr, G_arr, B_arr, h, w, out_fl_n))
		printf("Wtr OK\n");
	return;
}

extern "C"{
	void pyMatFmtLnW(const char *in_fl_n, const char *out_fl_n){
		MatFmtLnW(in_fl_n, out_fl_n);
	}
}

int main(){
	const char * in_fl_n = "tmp_mat_file";
	const char * out_fl_n = "out_tmp_mat_file";

	pyMatFmtLnW(in_fl_n, out_fl_n);

	return 0;
}
