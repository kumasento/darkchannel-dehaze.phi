/*
 * This one describes the picture class used in this application
 */

#include <string.h>

#include "FreeImage.h"
#include "FreeImageInterfaces.h"

typedef struct{
	unsigned rgbred;
	unsigned rgbgreen;
	unsigned rgbblue;
} rgbtuple;

class hazy_pixels{
	public:
		bool pixelsLoader(const char *file_name, int flag);

		void pixelsSetImageBasicInfo();
		unsigned pixelsGetHeight(){ return this->hazy_height; }
		unsigned pixelsGetWidth(){ return this->hazy_width; }
		rgbtuple* pixelsGetRGBTupleByCoord(unsigned x, unsigned y);
	private:
		FIBITMAP *hazy_bitmap;
		BITMAPINFO *hazy_bitmap_info;
		FREE_IMAGE_COLOR_TYPE hazy_bitmap_color_type;
		bool hazy_bitmap_available;
		unsigned hazy_height, hazy_width;
	
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
		return _rgbtuple;
	}
	return NULL;
}

