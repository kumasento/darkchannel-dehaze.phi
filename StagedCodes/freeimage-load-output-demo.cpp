#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "FreeImage.h"

using namespace std;

int main(int argc, char *argv[]){
	int opt;
	char file_name[60];

	while((opt = getopt(argc, argv, "f:")) != -1){
		switch(opt){
			case 'f':
				if(strlen(optarg) > 60){
					cerr << "ERROR: filename too long(60)" << endl;
					return 1;
				}
				file_name = optarg;
				break;
			default:
				cerr<<"Usage: -f [filename]"<<endl;
				return 1;
		}
	}

	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(file_name, 0);
	if(fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFileName(file_name);
	if((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif) ){
		FIBITMAP *bitmap = FreeImage_Load(fif, file_name, 0);
		if(!bitmap){
			cerr << "ERROR: Loading File" << endl;
			return 1;
		}
		//Save to BMP
		char output_file_name[60];
		memset(output_file_name, 0, sizeof(output_file_name));
		sprintf(output_file_name, "output_pixels_%d.bmp", 1);

		if(FreeImage_Save(FIF_BMP, bitmap, output_file_name, 0))
			cout << "INFO: Image Saved Successfully" << endl;
	}
	return 0;
}
