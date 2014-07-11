#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include "FreeImage.h"

using namespace std;

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

	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(file_name, 0);
	if(fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(file_name);

	//Get Format Info
	if(if_get_formatinfo)
		printf("%s Format\n", FreeImage_GetFormatFromFIF(fif)); 

	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif) ){
		FIBITMAP *bitmap = FreeImage_Load(fif, file_name, 0);
		if(!bitmap){
			cerr << "ERROR: Loading File" << endl;
			return 1;
		}
		//Save to BMP
		char output_file_name[60];
		memset(output_file_name, 0, sizeof(output_file_name));
		sprintf(output_file_name, "output_pixels_%d.%s", special_id, FreeImage_GetFormatFromFIF(fif));

		if(FreeImage_Save(fif, bitmap, output_file_name, 0))
			cout << "INFO: Image Saved Successfully" << endl;
	}
	return 0;
}
