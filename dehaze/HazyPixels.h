/*
 * This one describes the picture class used in this application
 */

#ifdef FREE_IMAGE_SUPPORT
	#include "FreeImage.h"
	#include "FreeImageInterfaces.h"
#elif OPENCV_SUPPORT
 	#include <OpenCVInterfaces.h>
#endif

#include <deque>

#define DEF_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define DEF_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define DEF_XYtoIdx(x, y, l) ((x) * (l) + (y))
#define DEF_XYZtoIdx(x, y, z, l, r) ((x) * (l) * (r) + (y) * (r) + (z))
#define DEF_COLOR_VALUE_MAX 256
#define DEF_FILE_NAME_LEN 60

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
		hazy_pixels(const char *file_name)
		{
			if(pixelsLoader(file_name, 0))
				puts("INFO: Loaded");
			pixelsSetImageBasicInfo();
		}
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

		void pixelsCalculate(int r, double eps);

		void pixelsPrintImageInfo();

		cv::Mat CV_IMAGE_RES_MAT;
		cv::Mat CV_IMAGE_MAT;
		unsigned hazy_height, hazy_width;
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
		char hazy_file_name[DEF_FILE_NAME_LEN];
};

bool hazy_pixels::pixelsLoader(const char *file_name, int flag = 0){
	this->hazy_name = std::string(file_name);
	int str_pos = this->hazy_name.find('.');
	this->hazy_name = this->hazy_name.substr(0, str_pos);

#ifdef FREE_IMAGE_SUPPORT
	this->hazy_bitmap = FIInterfaceGenericLoader(file_name, flag);
	if(this->hazy_bitmap == NULL)
		this->hazy_bitmap_available = false;
	else
		this->hazy_bitmap_available = true;

	strcpy(this->hazy_file_name, file_name);
	return this->hazy_bitmap_available;
#else
	return OpenCVInterfaces::CVImageLoader(file_name, this->CV_IMAGE_MAT);
#endif
}

void hazy_pixels::pixelsUnLoader(){
#ifdef FREE_IMAGE_SUPPORT
	FIInterfaceUnLoader(this->hazy_bitmap);
#endif
}

void hazy_pixels::pixelsSetDarkChannelValue(){
	int Idx = 0;
	for(unsigned x = 0; x < this->hazy_height; x++){
		for(unsigned y = 0; y < this->hazy_width; y++){
			pixel_array[Idx].dc_value = pixelsGetDarkChannelByCoord(x, y);
			Idx ++;
		}
	}
}

void hazy_pixels::pixelsSetImagePixelArray(){
	pixel_array = (hazy_pixel*) malloc( sizeof(hazy_pixel) * 
										this->hazy_width *
										this->hazy_height);

#ifdef FREE_IMAGE_SUPPORT
	int Idx = 0;
	for(unsigned x = 0; x < this->hazy_height; x++){
		for(unsigned y = 0;  y < this->hazy_width; y++){
			pixel_array[Idx].x = x;
			pixel_array[Idx].y = y;
			pixel_array[Idx].color_tuple = pixelsGetRGBTupleByCoord(x, y);
			//pixel_array[Idx].color_tuple = (rgbtuple*) malloc(sizeof(rgbtuple));
			
			/*
			rgbtuple *_tuple = pixelsGetRGBTupleByCoord(x, y);
			pixel_array[Idx].color_tuple->rgbred = _tuple->rgbred;
			pixel_array[Idx].color_tuple->rgbgreen = _tuple->rgbgreen;
			pixel_array[Idx].color_tuple->rgbblue = _tuple->rgbblue;
			*/

			pixel_array[Idx].dc_value = pixelsGetDarkChannelByCoord(x, y);

			Idx ++;
		}
	}
#elif OPENCV_SUPPORT
	elem_t * p = CV_IMAGE_MAT.data;
	int Idx = 0;
	for(unsigned x = 0; x < this->hazy_height; x++){
		for(unsigned y = 0; y < this->hazy_width; y++){
			pixel_array[Idx].x = x;
			pixel_array[Idx].y = y;
			pixel_array[Idx].color_tuple = (rgbtuple*) malloc(sizeof(rgbtuple));
			pixel_array[Idx].color_tuple->rgbblue = p[Idx*3];
			pixel_array[Idx].color_tuple->rgbgreen = p[Idx*3+1];
			pixel_array[Idx].color_tuple->rgbred = p[Idx*3+2];
			Idx++;
		}
	}
	/** 
	 * Here use the O(1) algorithm
	 */
	// Calculate the min value channel
	/*
	int color_min_chn[this->hazy_height][this->hazy_width];
	for(unsigned x = 0; x < this->hazy_height; x++){
		for(unsigned y = 0; y < this->hazy_width; y++){
			int Idx = x * this->hazy_width + y;
			int min_chn = pixel_array[Idx].color_tuple->rgbred;
			min_chn = DEF_MIN(min_chn, pixel_array[Idx].color_tuple->rgbgreen);
			min_chn = DEF_MIN(min_chn, pixel_array[Idx].color_tuple->rgbblue);
			color_min_chn[x][y] = min_chn;
		}
	}
	puts("INFO: color_min_chn done");

	// Calculate the min sequence (axis-x)
	int color_min[this->hazy_height][this->hazy_width];
	unsigned win_size = this->hazy_patch_size;
	printf("win size %d\n", win_size);
	for(unsigned x = 0; x < this->hazy_height; x++){
		std::deque<int> dq;
		for(unsigned y = 0; y < win_size; y++){
			while(!dq.empty() && color_min_chn[x][y] <= color_min_chn[x][dq.back()])
				dq.pop_back();
			dq.push_back(y);

			color_min[x][y] = color_min_chn[x][dq.front()];
		}
		for(unsigned y = win_size; y < this->hazy_width; y++){
			color_min[x][y] = color_min_chn[x][dq.front()];
			while(!dq.empty() && color_min_chn[x][y] <= color_min_chn[x][dq.back()])
				dq.pop_back();
			while (!dq.empty() && dq.front() <= y - win_size)  
   	   			dq.pop_front();  
    		dq.push_back(y);  
		}
	}
	puts("INFO: Calculate the min sequence (axis-x) done");

	//Calculate the min sequence (axis-y)
	for(unsigned x = 0; x < this->hazy_height; x++){
		
	}
	*/
#endif
}

/*
 * WARNING: Make sure you have loaded the bitmap
 */
void hazy_pixels::pixelsSetImageBasicInfo(){
#ifdef FREE_IMAGE_SUPPORT
	this->hazy_height = FreeImage_GetHeight(this->hazy_bitmap);
	this->hazy_width = FreeImage_GetWidth(this->hazy_bitmap);
	this->hazy_bitmap_info = FreeImage_GetInfo(this->hazy_bitmap);
	this->hazy_bitmap_color_type = FreeImage_GetColorType(this->hazy_bitmap);
#elif OPENCV_SUPPORT
	this->hazy_height = this->CV_IMAGE_MAT.rows;
	this->hazy_width = this->CV_IMAGE_MAT.cols;
#endif
}

rgbtuple* hazy_pixels::pixelsGetRGBTupleByCoord(unsigned x, unsigned y){

#ifdef FREE_IMAGE_SUPPORT
	RGBQUAD *rgbptr = (RGBQUAD*) malloc(sizeof(RGBQUAD));
	if(FreeImage_GetPixelColor(this->hazy_bitmap, x, y, rgbptr)){
		rgbtuple *_rgbtuple = (rgbtuple*) malloc(sizeof(rgbtuple));
		_rgbtuple->rgbred = rgbptr->rgbRed;
		_rgbtuple->rgbgreen = rgbptr->rgbGreen;
		_rgbtuple->rgbblue = rgbptr->rgbBlue;
		/*
		printf("[%u %u %u]", rgbptr->rgbRed,
							 rgbptr->rgbGreen,
					         rgbptr->rgbBlue);
		*/
		return _rgbtuple;
	}
#endif


	return NULL;
}



void hazy_pixels::pixelsPrintImageInfo(){
	printf("Image Info:\n");
	printf("name: %s\n", this->hazy_name.c_str());
	printf("height: %d width: %d\n", this->hazy_height, this->hazy_width);
	return ;
}
