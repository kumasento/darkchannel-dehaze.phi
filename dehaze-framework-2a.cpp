#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string>

#include "HazyPixels.h"
#include "HazyPixelsSave.h"

using namespace std;

int main(int argc, char *argv[]){
	int opt;
	string file_name;
	bool show_info = false;
	int patch_size = 3;

	while((opt = getopt(argc, argv, "f:iw:")) != -1){
		switch(opt){
			case 'f':
				file_name = string(optarg);
				break;
			case 'i':
				show_info = true;
				break;
			case 'w':
				patch_size = atoi(optarg);
				break;
			default:
				return 1;
		}
	}

	hazy_pixels hpixels;
	bool isLoaded = hpixels.pixelsLoader(file_name.c_str(), 0);
	if(isLoaded)
		cout <<"INFO: Loaded" << endl;
	hpixels.pixelsSetImageBasicInfo();
	
	if(show_info){
		printf("Picture Name: %s\n", file_name.c_str());
		printf("Height: %u Width: %u\n", hpixels.pixelsGetHeight(),
										 hpixels.pixelsGetWidth());
	}

	hpixels.pixelsSetImagePatchSize( patch_size );
	
	clock_t start = clock();
	hpixels.pixelsSaveImageMattedOriginalBitmap();
	clock_t end = clock();
	double duration = (double)(end-start) / CLOCKS_PER_SEC;
	printf("1-time duration: %f sec\n", duration);

	hpixels.pixelsUnLoader();
	cout << "INFO: UnLoaded" << endl;

	//FIBITMAP *pic_bitmap = GenericLoader(file_name.c_str(), 0);

	return 0;
}
