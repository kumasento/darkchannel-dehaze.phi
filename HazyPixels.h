/*
 * This one describes the picture class used in this application
 */

#ifdef FREE_IMAGE_SUPPORT
	#include "FreeImage.h"
	#include "FreeImageInterfaces.h"
#elif OPENCV_SUPPORT
 	#include <OpenCVInterfaces.h>
#endif

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

int hazy_pixel_cmp(const void* a, const void *b){
	return ((hazy_pixel*)a)->dc_value - ((hazy_pixel*)b)->dc_value;
}

class hazy_pixels{
	public:
		hazy_pixels();
		hazy_pixels(const char *file_name)
		{
			pixelsLoader(file_name, 0);
			pixelsSetImageBasicInfo();
			pixelsSetImagePixelArray();
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
		double pixelsGetOriginaltValueByCoord(unsigned x, unsigned y);
		void pixelsBuildtValueArray(int r, double eps);

		void pixelsSaveOriginalImage(const char *file_name);
		void pixelsSaveImageDarkChannelBitmap();
		void pixelsSaveImageRawOriginalBitmap();
		void pixelsSaveImageMattedOriginalBitmap(int r, double eps);

		void pixelsPrintImageInfo();
	private:
	#ifdef FREE_IMAGE_SUPPORT
		FIBITMAP *hazy_bitmap;
		BITMAPINFO *hazy_bitmap_info;

		FREE_IMAGE_COLOR_TYPE hazy_bitmap_color_type;
		bool hazy_bitmap_available;
	#elif OPENCV_SUPPORT
		cv::Mat CV_IMAGE_MAT;
	#endif
		unsigned hazy_height, hazy_width;
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

void hazy_pixels::pixelsSetImageAtmosphereLightValue(){
	pixelsSetImagePixelArray();
	
	qsort(this->pixel_array, this->hazy_width *
							 this->hazy_height,
							 sizeof(this->pixel_array[0]),
							 hazy_pixel_cmp);

	unsigned array_upper_bound = (unsigned)(0.0001 * 
											(double) this->hazy_width * 
												   	 this->hazy_height);
	if(array_upper_bound == 0) array_upper_bound = DEF_MIN(10, 
													this->hazy_width *
													this->hazy_height);
	

	double red_per = 0.33;
	double blue_per = 0.33;
	double green_per = 0.33;

	
	//Use average result:
	double itn = 0.0;
	for(unsigned i = 0; i < array_upper_bound; i++){
		itn += red_per * this->pixel_array[i].color_tuple->rgbred + 
			   green_per * this->pixel_array[i].color_tuple->rgbgreen + 
			   blue_per * this->pixel_array[i].color_tuple->rgbblue;
	}
	itn /= array_upper_bound;
	this->A_value = (rgbtuple *) malloc(sizeof(rgbtuple));
	this->A_value->rgbred = this->A_value->rgbgreen = this->A_value->rgbblue = itn;
	
	printf("A: [%u %u %u]\n",	this->A_value->rgbred,
								this->A_value->rgbgreen,
								this->A_value->rgbblue);
	free(this->pixel_array);
	return ;
}

byte hazy_pixels::pixelsGetDarkChannelByCoord(unsigned x, unsigned y){
	// default is floor
	unsigned half_patch_size = (unsigned)this->hazy_patch_size / 2;
	unsigned start_posx, start_posy, end_posx, end_posy;
	if(this->hazy_patch_size % 2 == 0)
		half_patch_size --;

	if(x < half_patch_size){ start_posx = 0; end_posx = x + half_patch_size; }
	else{ start_posx = x - half_patch_size; end_posx = x + half_patch_size; }
	if(y < half_patch_size){ start_posy = 0; end_posy = y + half_patch_size; }
	else{ start_posy = y - half_patch_size; end_posy = y + half_patch_size; }

	if(end_posx > this->hazy_width){
		end_posx = this->hazy_width;
		start_posx = (start_posx > half_patch_size) ?
					 start_posx - half_patch_size :
					 0;
	}
	if(end_posy > this->hazy_height){
		end_posy = this->hazy_height;
		start_posy = (start_posy > half_patch_size) ?
					 start_posy - half_patch_size :
					 0;
	}

	byte darkChannelValue = DEF_COLOR_VALUE_MAX-1; 
	for(unsigned tmp_posx = start_posx; tmp_posx < end_posx; tmp_posx++){
		for(unsigned tmp_posy = start_posy; tmp_posy < end_posy; tmp_posy++){
			rgbtuple * _rgbtuple = (rgbtuple*) malloc(sizeof(rgbtuple));
			_rgbtuple = pixelsGetRGBTupleByCoord(tmp_posx, tmp_posy);
			/*
			printf("(%u %u): ", tmp_posx, tmp_posy);
			printf("[%u %u %u]", _rgbtuple->rgbred,
								 _rgbtuple->rgbgreen,
							     _rgbtuple->rgbblue);
			*/
			byte _darkChannelValue = _rgbtuple->rgbred;
			_darkChannelValue = DEF_MIN(_rgbtuple->rgbgreen, _darkChannelValue);
			_darkChannelValue = DEF_MIN(_rgbtuple->rgbblue, _darkChannelValue);
			darkChannelValue = DEF_MIN(darkChannelValue, _darkChannelValue);
		}
	}
	return darkChannelValue;
}

double hazy_pixels::pixelsGetOriginaltValueByCoord(unsigned x, unsigned y){
	unsigned half_patch_size = (unsigned)this->hazy_patch_size / 2;
	unsigned start_posx, start_posy, end_posx, end_posy;
	if(this->hazy_patch_size % 2 == 0)
		half_patch_size --;

	if(x < half_patch_size){ start_posx = 0; end_posx = x + half_patch_size; }
	else{ start_posx = x - half_patch_size; end_posx = x + half_patch_size; }
	if(y < half_patch_size){ start_posy = 0; end_posy = y + half_patch_size; }
	else{ start_posy = y - half_patch_size; end_posy = y + half_patch_size; }

	if(end_posx > this->hazy_width){
		end_posx = this->hazy_width;
		start_posx = (start_posx > half_patch_size) ?
					 start_posx - half_patch_size :
					 0;
	}
	if(end_posy > this->hazy_height){
		end_posy = this->hazy_height;
		start_posy = (start_posy > half_patch_size) ?
					 start_posy - half_patch_size :
					 0;
	}

	double tValue = 1.0; 
	for(unsigned tmp_posx = start_posx; tmp_posx < end_posx; tmp_posx++){
		for(unsigned tmp_posy = start_posy; tmp_posy < end_posy; tmp_posy++){
			rgbtuple * _rgbtuple = (rgbtuple*) malloc(sizeof(rgbtuple));
			_rgbtuple = pixelsGetRGBTupleByCoord(tmp_posx, tmp_posy);
			/*
			printf("(%u %u): ", tmp_posx, tmp_posy);
			printf("[%u %u %u]", _rgbtuple->rgbred,
								 _rgbtuple->rgbgreen,
							     _rgbtuple->rgbblue);
			*/
			double _tValue = (double)_rgbtuple->rgbred/this->A_value->rgbred;
			_tValue = DEF_MIN((double)_rgbtuple->rgbgreen/this->A_value->rgbgreen,
							  _tValue);
			_tValue = DEF_MIN((double)_rgbtuple->rgbblue/this->A_value->rgbblue, 
							  _tValue);
			tValue = DEF_MIN(tValue, _tValue);
		}
	}

	//printf("%lf\n", 1-0.95*tValue);
	
	return 1 - 0.95*tValue;
}

void hazy_pixels::pixelsPrintImageInfo(){
	printf("Image Info:\n");
	printf("name: %s\n", this->hazy_name.c_str());
	printf("height: %d width: %d\n", this->hazy_height, this->hazy_width);
	return ;
}
