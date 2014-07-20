/*
 * This one describes the picture class used in this application
 */

#ifdef FREE_IMAGE_SUPPORT
	#include "FreeImage.h"
	#include "FreeImageInterfaces.h"
#elif OpenCV_Sprt
 	#include <OpenCVInterfaces.h>
#endif

#include <MatFmtInterfaces.h>

#include <Filter.h>

#define DEF_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define DEF_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define DEF_XYtoIdx(x, y, l) ((x) * (l) + (y))
#define DEF_XYZtoIdx(x, y, z, l, r) ((x) * (l) * (r) + (y) * (r) + (z))
#define DEF_COLOR_VALUE_MAX 256
#define DEF_FILE_NAME_LEN 60

#define MALLOC malloc

typedef unsigned char byte;

typedef struct{
	unsigned rgbred;
	unsigned rgbgreen;
	unsigned rgbblue;
} rgbtuple;

typedef struct{
	rgbtuple *color_tuple;
	byte dc_value;
	unsigned x, y;
} hazy_pixel;


class hazy_pixels{
	public:
		hazy_pixels();
		// load from a file. I think this one should be aborted
		hazy_pixels(const char *file_name){
			pixelsLoader(file_name, 0);
		}

		bool pixelsLoader(const char *file_name, int flag);
		void pixelsUnLoader();

		void pixelsSetImageBasicInfo();
		void pixelsSetImagePatchSize(int patch_size){ this->hazy_patch_size = 
													  patch_size; }
		void pixelsSetImagePixelArray();
		void pixelsConvertRGBPixelArray(byte *R_mat, byte *G_mat, byte* B_mat);
		void pixelsSetImageAtmosphereLightValue();
		unsigned pixelsGetHeight(){ return this->hazy_height; }
		unsigned pixelsGetWidth(){ return this->hazy_width; }
		rgbtuple* pixelsGetRGBTupleByCoord(unsigned x, unsigned y);
		byte pixelsGetDarkChannelByCoord(unsigned x, unsigned y);
		void pixelsSetDarkChannelValue();
		double pixelsGetOriginaltValueByCoord(unsigned x, unsigned y);
		void pixelsBuildtValueArray(int r, double eps);

		void pixelsSaveOriginalImage(const char *file_name);
		void pixelsSaveImageDarkChannelBitmap();
		void pixelsSaveImageRawOriginalBitmap();
		void pixelsSaveImageMattedOriginalBitmap(int r, double eps);

		void pixelsSetResultMat(byte* R_arr, byte* G_arr, byte* B_arr);
		void pixelsSaveResult(const char* out_fl_n, byte* R_arr, byte* G_arr, byte* B_arr);
		void pixelsCalculate(int r, double eps, byte* R_arr, byte* G_arr, byte* B_arr);

		void pixelsPrintImageInfo();

		unsigned hazy_height, hazy_width;
		unsigned hazy_pixel_size;

	private:
		unsigned hazy_patch_size;	

		hazy_pixel *pixel_array;
		rgbtuple *A_value;
		double *t_value_arr;
};

bool hazy_pixels::pixelsLoader(const char *file_name, int flag = 0){
	int MatFmt_H, MatFmt_W;
	if(MatFmt::MatFmtHWLdr(MatFmt_H, MatFmt_W, file_name)){
		printf("Loaded: Height %d Width %d\n", MatFmt_H, MatFmt_W);

		this->hazy_width = MatFmt_W;
		this->hazy_height = MatFmt_H;
		this->hazy_pixel_size = MatFmt_W * MatFmt_H;

		byte* tmp_R_arr = (byte*) MALLOC(sizeof(byte) * MatFmt_H * MatFmt_W);
		byte* tmp_G_arr = (byte*) MALLOC(sizeof(byte) * MatFmt_H * MatFmt_W);
		byte* tmp_B_arr = (byte*) MALLOC(sizeof(byte) * MatFmt_H * MatFmt_W);

		if(MatFmt::MatFmtRGBLdr(tmp_R_arr, tmp_G_arr, tmp_B_arr, MatFmt_H, MatFmt_W, file_name)){
			printf("Loaded: R G B array\n");

			// first step assign the color tuple
			pixel_array = (hazy_pixel*) MALLOC( sizeof(hazy_pixel) * 
												this->hazy_width *
												this->hazy_height);
			for(unsigned x = 0; x < this->hazy_height; x++){
				for(unsigned y = 0; y < this->hazy_width; y++){
					int Idx = x * this->hazy_width + y;
					pixel_array[Idx].x = x;
					pixel_array[Idx].y = y;
					pixel_array[Idx].color_tuple = (rgbtuple*) MALLOC(sizeof(rgbtuple));
					pixel_array[Idx].color_tuple->rgbblue = tmp_B_arr[Idx];
					pixel_array[Idx].color_tuple->rgbgreen = tmp_G_arr[Idx];
					pixel_array[Idx].color_tuple->rgbred = tmp_R_arr[Idx];
				}
			}
		}

		else{
			puts("ERROR: Cant Load RGB Array");
			return false;
		}
	}
	else{
		puts("ERROR: Cant Load H n W");
		return false;
	}

	return false;
}

void hazy_pixels::pixelsSaveResult(const char* out_fl_n, byte* R_arr, byte* G_arr, byte* B_arr){
	MatFmt::MatFmtRGBWtr(R_arr, G_arr, B_arr, hazy_height, hazy_width, out_fl_n);
}

void hazy_pixels::pixelsSetDarkChannelValue(){
	byte* R_mat = (byte *) MALLOC( sizeof(byte) * this->hazy_pixel_size); 
	byte* G_mat = (byte *) MALLOC( sizeof(byte) * this->hazy_pixel_size); 
	byte* B_mat = (byte *) MALLOC( sizeof(byte) * this->hazy_pixel_size); 

	this->pixelsConvertRGBPixelArray(R_mat, G_mat, B_mat);

	byte* input_2d_arr = (byte *) MALLOC( sizeof(byte) * this->hazy_pixel_size );
	byte* win_2d_arr = (byte *) MALLOC( sizeof(byte) * this->hazy_pixel_size );
	byte* tran_2d_arr = (byte *) MALLOC( sizeof(byte) * this->hazy_pixel_size );
	
	Filter::ComputeMinRGBArray(R_mat, G_mat, B_mat, input_2d_arr, this->hazy_pixel_size);
	Filter::Compute2DMinWindowFilterArray(input_2d_arr, win_2d_arr, tran_2d_arr, this->hazy_height, this->hazy_width, this->hazy_patch_size/2);

	for(unsigned x = 0; x < this->hazy_height; x ++)
		for(unsigned y = 0; y < this->hazy_width; y++)
			pixel_array[x*this->hazy_width+y].dc_value = tran_2d_arr[y*this->hazy_height+x];
}

void hazy_pixels::pixelsConvertRGBPixelArray(byte * R_mat, byte * G_mat, byte * B_mat){

	for(unsigned idx = 0; idx < this->hazy_pixel_size; idx ++){
		R_mat[idx] = this->pixel_array[idx].color_tuple->rgbred;
		G_mat[idx] = this->pixel_array[idx].color_tuple->rgbgreen;
		B_mat[idx] = this->pixel_array[idx].color_tuple->rgbblue;
	}
}
