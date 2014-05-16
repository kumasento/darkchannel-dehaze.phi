#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string>

#include "FreeImage.h"
#include "HazyPixels.h"

using namespace std;

int main(int argc, char *argv[]){
	int opt;
	string file_name;
	bool show_info = false;

	while((opt = getopt(argc, argv, "f:i")) != -1){
		switch(opt){
			case 'f':
				file_name = string(optarg);
				break;
			case 'i':
				show_info = true;
				break;
			default:
				return 1;
		}
	}

	hazy_pixels hpixels;
	hpixels.pixelsLoader(file_name.c_str(), 0);
	hpixels.pixelsSetImageBasicInfo();
	
	if(show_info){
		printf("Picture Name: %s\n", file_name.c_str());
		printf("Height: %u Width: %u\n", hpixels.pixelsGetHeight(),
										 hpixels.pixelsGetWidth());
	}

	//FIBITMAP *pic_bitmap = GenericLoader(file_name.c_str(), 0);

	return 0;
}
