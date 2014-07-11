#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "FreeImage.h"

#define WIDTH 800
#define HEIGHT 600
#define BPP 24

using namespace std;

int main(int argc, char *argv[]){
	int bpp = BPP;
	int oc;
	while((oc = getopt(argc, argv, "b:"))!=-1){ 
		switch(oc){
			case 'b':
				bpp = atoi(optarg);
				break;
			default:
				break;
		}
	}

	FreeImage_Initialise();

	FIBITMAP * bitmap = FreeImage_Allocate(WIDTH, HEIGHT, bpp);
	RGBQUAD color;

	if(!bitmap)
		return 1;
	for( int i = 0; i < WIDTH; i++){
		for(int j = 0; j < HEIGHT; j++){
			color.rgbRed = 0;
			color.rgbGreen = (double) i / WIDTH * 255.0;
			color.rgbBlue = (double) i / HEIGHT * 255.0;
			FreeImage_SetPixelColor(bitmap, i, j, &color);
		}
	}

	char opng_name[60]; memset(opng_name, 0, sizeof(opng_name));
	sprintf(opng_name, "complete_demo_%d.png", bpp);

	if( FreeImage_Save(FIF_PNG, bitmap, opng_name, 0) )
		cout << "Image Successfully Saved!" << endl;
	FreeImage_DeInitialise();

	return 0;
}
