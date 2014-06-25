/*
 * This one describes the picture class used in this application
 */

#include "FreeImage.h"
#include "FreeImageInterfaces.h"
#include "utils.h"

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
		void pixelsBuildtValueArray();

		void pixelsSaveImageDarkChannelBitmap();
		void pixelsSaveImageRawOriginalBitmap();
		void pixelsSaveImageMattedOriginalBitmap();
	private:
		FIBITMAP *hazy_bitmap;
		BITMAPINFO *hazy_bitmap_info;

		FREE_IMAGE_COLOR_TYPE hazy_bitmap_color_type;
		bool hazy_bitmap_available;
		unsigned hazy_height, hazy_width;
		unsigned hazy_patch_size;	

		hazy_pixel *pixel_array;
		rgbtuple *A_value;
		double *t_vale_arr;
		char hazy_file_name[DEF_FILE_NAME_LEN];
};

bool hazy_pixels::pixelsLoader(const char *file_name, int flag){
	this->hazy_bitmap = FIInterfaceGenericLoader(file_name, flag);
	if(this->hazy_bitmap == NULL)
		this->hazy_bitmap_available = false;
	else
		this->hazy_bitmap_available = true;

	strcpy(this->hazy_file_name, file_name);
	return this->hazy_bitmap_available;
}

void hazy_pixels::pixelsUnLoader(){
	FIInterfaceUnLoader(this->hazy_bitmap);
}

void hazy_pixels::pixelsSetImagePixelArray(){
	pixel_array = (hazy_pixel*) malloc( sizeof(hazy_pixel) * 
										this->hazy_width *
										this->hazy_height);
	int Idx = 0;
	for(unsigned x = 0; x < this->hazy_width; x++){
		for(unsigned y = 0;  y < this->hazy_height; y++){
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
}

/*
 * WARNING: Make sure you have loaded the bitmap
 */
void hazy_pixels::pixelsSetImageBasicInfo(){
	this->hazy_height = FreeImage_GetHeight(this->hazy_bitmap);
	this->hazy_width = FreeImage_GetWidth(this->hazy_bitmap);
	this->hazy_bitmap_info = FreeImage_GetInfo(this->hazy_bitmap);
	this->hazy_bitmap_color_type = FreeImage_GetColorType(this->hazy_bitmap);
}

rgbtuple* hazy_pixels::pixelsGetRGBTupleByCoord(unsigned x, unsigned y){
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

	/*
	double tmp_itnsty = 0;
	unsigned tmp_Idx = 0;
	for(unsigned i = 0; i < array_upper_bound; i++){
		double itn =   red_per * this->pixel_array[i].color_tuple->rgbred + 
					   green_per * this->pixel_array[i].color_tuple->rgbgreen + 
					   blue_per * this->pixel_array[i].color_tuple->rgbblue;
		if(itn > tmp_itnsty){
			tmp_itnsty = itn;
			tmp_Idx = i;
		}
	}

	this->A_value = (rgbtuple *) malloc(sizeof(rgbtuple));
	this->A_value->rgbred = this->pixel_array[tmp_Idx].color_tuple->rgbred;
	this->A_value->rgbgreen = this->pixel_array[tmp_Idx].color_tuple->rgbgreen;
	this->A_value->rgbblue = this->pixel_array[tmp_Idx].color_tuple->rgbblue;

	//WARNING
	double trans =(double) ( red_per * this->A_value->rgbred + 
							 green_per * this->A_value->rgbgreen + 
							 blue_per * this->A_value->rgbblue);
	this->A_value->rgbred = 
	this->A_value->rgbblue = 
	this->A_value->rgbgreen = (byte) trans;
	printf("(%u %u) [%u %u %u]\n", this->pixel_array[tmp_Idx].x,
								   this->pixel_array[tmp_Idx].y,
								   this->A_value->rgbred,
								   this->A_value->rgbgreen,
								   this->A_value->rgbblue);
	*/
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
