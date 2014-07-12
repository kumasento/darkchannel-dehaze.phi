/*
 * This one describes the picture class used in this application
 */

#ifdef FREE_IMAGE_SUPPORT
	#include "FreeImage.h"
	#include "FreeImageInterfaces.h"
#elif OPENCV_SUPPORT
 	#include <OpenCVInterfaces.h>
#endif

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
		hazy_pixels(const char *file_name)
		{
			if(pixelsLoader(file_name, 0))
				puts("INFO: Loaded");
			pixelsSetImageBasicInfo();
		}

		// Load from OpenCV Mat, this one should be aborted also
		// Load from a data structure that has no relation with the file system or library
		hazy_pixels(cv::Mat& mat){
			this->CV_IMAGE_MAT = mat;
			pixelsSetImageBasicInfo();
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

		void pixelsSetResultMat();
		void pixelsCalculate(int r, double eps);

		void pixelsPrintImageInfo();

		void pixelsConfigure(int gvar_dc_type = 0){
			this->gvar_dc_type = gvar_dc_type;
		};

		cv::Mat CV_IMAGE_RES_MAT;
		cv::Mat CV_IMAGE_MAT;
		unsigned hazy_height, hazy_width;
		unsigned hazy_pixel_size;
		//computation configure
		int gvar_dc_type;
	private:
	#ifdef FREE_IMAGE_SUPPORT
		FIBITMAP *hazy_bitmap;
		BITMAPINFO *hazy_bitmap_info;

		FREE_IMAGE_COLOR_TYPE hazy_bitmap_color_type;
		bool hazy_bitmap_available;
	#endif
		
		unsigned hazy_patch_size;	
		std::string hazy_name;


		hazy_pixel *pixel_array;
		rgbtuple *A_value;
		double *t_value_arr;

};

bool hazy_pixels::pixelsLoader(const char *file_name, int flag = 0){
	this->hazy_name = std::string(file_name);
	int str_pos = this->hazy_name.find('.');
	this->hazy_name = this->hazy_name.substr(0, str_pos);

	return OpenCVInterfaces::CVImageLoader(file_name, this->CV_IMAGE_MAT);
}

void hazy_pixels::pixelsUnLoader(){
}

void hazy_pixels::pixelsSetDarkChannelValue(){
	if(this->gvar_dc_type == 0){
		int Idx = 0;
		for(unsigned x = 0; x < this->hazy_height; x++){
			for(unsigned y = 0; y < this->hazy_width; y++){
				pixel_array[Idx].dc_value = pixelsGetDarkChannelByCoord(x, y);
				Idx ++;
			}
		}
	}	
	else if(this->gvar_dc_type == 1){
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
}

void hazy_pixels::pixelsSetImagePixelArray(){
	pixel_array = (hazy_pixel*) MALLOC( sizeof(hazy_pixel) * 
										this->hazy_width *
										this->hazy_height);
	elem_t * p = CV_IMAGE_MAT.data;

	for(unsigned x = 0; x < this->hazy_height; x++){
		for(unsigned y = 0; y < this->hazy_width; y++){
			int Idx = x * this->hazy_width + y;
			pixel_array[Idx].x = x;
			pixel_array[Idx].y = y;
			pixel_array[Idx].color_tuple = (rgbtuple*) MALLOC(sizeof(rgbtuple));
			pixel_array[Idx].color_tuple->rgbblue = p[Idx*3];
			pixel_array[Idx].color_tuple->rgbgreen = p[Idx*3+1];
			pixel_array[Idx].color_tuple->rgbred = p[Idx*3+2];
		}
	}
}

void hazy_pixels::pixelsConvertRGBPixelArray(byte * R_mat, byte * G_mat, byte * B_mat){

	for(unsigned idx = 0; idx < this->hazy_pixel_size; idx ++){
		R_mat[idx] = this->pixel_array[idx].color_tuple->rgbred;
		G_mat[idx] = this->pixel_array[idx].color_tuple->rgbgreen;
		B_mat[idx] = this->pixel_array[idx].color_tuple->rgbblue;
	}
}

/*
 * WARNING: Make sure you have loaded the bitmap
 */
void hazy_pixels::pixelsSetImageBasicInfo(){
	this->hazy_height = this->CV_IMAGE_MAT.rows;
	this->hazy_width = this->CV_IMAGE_MAT.cols;
	this->hazy_pixel_size = this->hazy_height * this->hazy_width;
}

rgbtuple* hazy_pixels::pixelsGetRGBTupleByCoord(unsigned x, unsigned y){
	return NULL;
}



void hazy_pixels::pixelsPrintImageInfo(){
	printf("Image Info:\n");
	printf("name: %s\n", this->hazy_name.c_str());
	printf("height: %d width: %d\n", this->hazy_height, this->hazy_width);
	return ;
}
