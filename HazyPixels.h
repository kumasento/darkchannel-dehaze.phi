/*
 * This one describes the picture class used in this application
 */

#include "FreeImage.h"
#include "FreeImageInterfaces.h"
#include "utils.h"

#define DEF_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define DEF_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define DEF_XYtoIdx(x, y, l) ((x) * (l) + (y))
#define DEF_COLOR_VALUE_MAX 256

typedef struct{
	unsigned rgbred;
	unsigned rgbgreen;
	unsigned rgbblue;
} rgbtuple;
typedef unsigned char byte;
class hazy_pixels{
	public:
		bool pixelsLoader(const char *file_name, int flag);

		void pixelsSetImageBasicInfo();
		void pixelsSetImagePatchSize(int patch_size){ this->hazy_patch_size = 
													  patch_size; }
		void pixelsSetImageDarkChannelArray();
		unsigned pixelsGetHeight(){ return this->hazy_height; }
		unsigned pixelsGetWidth(){ return this->hazy_width; }
		rgbtuple* pixelsGetRGBTupleByCoord(unsigned x, unsigned y);
		byte pixelsGetDarkChannelByCoord(unsigned x, unsigned y);
		
		void pixelsSaveImageDarkChannelBitmap();
	private:
		FIBITMAP *hazy_bitmap;
		BITMAPINFO *hazy_bitmap_info;
		byte *hazy_darkchannel_array;

		FREE_IMAGE_COLOR_TYPE hazy_bitmap_color_type;
		bool hazy_bitmap_available;
		unsigned hazy_height, hazy_width;
		unsigned hazy_patch_size;	
};

bool hazy_pixels::pixelsLoader(const char *file_name, int flag){
	this->hazy_bitmap = FIInterfaceGenericLoader(file_name, flag);
	if(this->hazy_bitmap == NULL)
		this->hazy_bitmap_available = false;
	else
		this->hazy_bitmap_available = true;
	return this->hazy_bitmap_available;
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

/*
 * WARNING: Make sure you've set all the info
 */ 

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

void hazy_pixels::pixelsSetImageDarkChannelArray(){
	unsigned array_size = this->hazy_height * this->hazy_width;
	this->hazy_darkchannel_array = (byte *) malloc(sizeof(byte) * array_size);

	for(unsigned x = 0; x < this->hazy_width; x++){
		for(unsigned y = 0; y < this->hazy_height; y++){
			unsigned Idx = DEF_XYtoIdx(x, y, this->hazy_width);
			hazy_darkchannel_array[Idx] = pixelsGetDarkChannelByCoord(x, y);
		}
	}

}

void hazy_pixels::pixelsSaveImageDarkChannelBitmap(){
	char file_name[60]; memset(file_name, 0, sizeof(file_name));
	sprintf(file_name, "dark_channel_bitmap_%u.png", this->hazy_patch_size);
	FIBITMAP *dcBitmap = FIInterfaceGenerateBitmapEightBits(this->hazy_width,
															this->hazy_height);

	for(unsigned x = 0; x < this->hazy_width; x++){
		for(unsigned y = 0; y < this->hazy_height; y++){
			byte dcValue = pixelsGetDarkChannelByCoord(x, y);
			//printf("(%u %u) %u ", x, y, dcValue);
			FreeImage_SetPixelIndex(dcBitmap,
									x,
									y,
									&dcValue);
		}
	}

	FreeImage_Save(FIF_PNG, dcBitmap, file_name, 0);
}
