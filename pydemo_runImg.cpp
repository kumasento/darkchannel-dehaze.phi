#include <iostream>

#include <HazyPixels.h>
#include <HazyPixelsCompute.h>

void RunImgDehaze(const char *in_fl_n, int patch_size, const char *out_fl_n)
{
	hazy_pixels hp(in_fl_n);
	hp.pixelsSetImagePatchSize(patch_size);

	int h = hp.hazy_height; 
	int w = hp.hazy_width;
	
	byte* R_arr = (byte*) malloc(sizeof(byte) * h * w);
	byte* G_arr = (byte*) malloc(sizeof(byte) * h * w);
	byte* B_arr = (byte*) malloc(sizeof(byte) * h * w);

	int r = 30;
	double e = 1e-3;
	hp.pixelsCalculate(r, e, R_arr, G_arr, B_arr);

	hp.pixelsSaveResult(out_fl_n, R_arr, G_arr, B_arr);

	return ;
}

extern "C"
{
	void pyRunImgDehaze(const char *in_fl_n, int patch_size, const char *out_fl_n){
		RunImgDehaze(in_fl_n, patch_size, out_fl_n);
	}
}

int main(){
	const char *in_fl_n = "PyLib/tmp_mat_file";
	const char *out_fl_n = "PyLib/out_tmp_mat_file";
	int patch_num = 3;

	pyRunImgDehaze(in_fl_n, patch_num, out_fl_n);

	return 0;
}
