#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include "FreeImage.h"

using namespace std;

/*
 * This is the generic image loader from the 
 * FreeImage's document
 */
FIBITMAP * GenericLoader(const char * file_name, int flag){
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(file_name, 0);
	if(fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(file_name);

	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif) ){
		FIBITMAP *bitmap = FreeImage_Load(fif, file_name, 0);
		return bitmap;
	}
	return NULL;
}

void GenericImageInfo(FIBITMAP * bitmap){
	FREE_IMAGE_TYPE img_type = FreeImage_GetImageType(bitmap);
	unsigned color_used = FreeImage_GetColorsUsed(bitmap);
	unsigned bpp = FreeImage_GetBPP(bitmap);
	unsigned height = FreeImage_GetHeight(bitmap);
	unsigned width = FreeImage_GetWidth(bitmap);
	unsigned dib_size = FreeImage_GetDIBSize(bitmap);
	BITMAPINFO * bitmap_info = FreeImage_GetInfo(bitmap);
	FREE_IMAGE_COLOR_TYPE color_type = FreeImage_GetColorType(bitmap);
	unsigned red_mask = FreeImage_GetRedMask(bitmap);
	unsigned blue_mask = FreeImage_GetBlueMask(bitmap);
	unsigned green_mask = FreeImage_GetGreenMask(bitmap);
	unsigned transparent_count = FreeImage_GetTransparencyCount(bitmap);

	printf("Height: %u Width: %u\n", height, width);	
	printf("Mask:\nred: %u\nblue: %u\ngreen: %u\n", red_mask, blue_mask, green_mask);

	//Pixel
	cout << "Get Pixel Color List" << endl;
	for(unsigned h = 0; h < height; h++){
		for(unsigned w = 0; w < width; w++){
			printf("(%u, %u): ", h, w);
			RGBQUAD *rgbptr = (RGBQUAD*) malloc(sizeof(RGBQUAD)); 
			if(FreeImage_GetPixelColor(bitmap, w, h, rgbptr)){
				printf("[%u %u %u]\n", rgbptr->rgbRed,
									   rgbptr->rgbBlue,
									   rgbptr->rgbGreen);
			}
			else{
				cerr<<"ERROR: pixel access denied" << endl;
			}
		}
	}
}

int main(int argc, char *argv[]){
	int opt;
	char file_name[60];
	bool if_get_formatinfo = false;
	int special_id = 1;
	while((opt = getopt(argc, argv, "f:iv:")) != -1){
		switch(opt){
			case 'f':
				if(strlen(optarg) > 60){
					cerr << "ERROR: filename too long(60)" << endl;
					return 1;
				}
				strcpy(file_name, optarg);
				break;
			case 'i':
				if_get_formatinfo = true;
				break;
			case 'v':
				special_id = atoi(optarg);
				break;
			default:
				cerr<<"Usage: -f [filename]"<<endl;
				return 1;
				break;
		}
	}
	FreeImage_Initialise();
	FIBITMAP *bitmap = GenericLoader(file_name, 0);
	if(bitmap == NULL){
		cerr << "ERROR: bitmap broke" << endl;
		return 1;
	}
	cout << "INFO: image loaded" << endl;

	if(if_get_formatinfo)
		GenericImageInfo(bitmap);

	FreeImage_Unload(bitmap);
	cout << "INFO: image unloaded" << endl;
	FreeImage_DeInitialise();
	return 0;
}
