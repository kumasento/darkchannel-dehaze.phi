#include "FreeImage.h"

FIBITMAP * FIInterfaceGenericLoader(const char * file_name, int flag){
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(file_name, 0);
	if(fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(file_name);

	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif) ){
		FIBITMAP *bitmap = FreeImage_Load(fif, file_name, 0);
		return bitmap;
	}
	return NULL;
}
