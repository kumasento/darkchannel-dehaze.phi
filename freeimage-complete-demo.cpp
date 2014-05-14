#include <iostream>
#include "FreeImage.h"

#define WIDTH 800
#define HEIGHT 600
#define BPP 24

using namespace std;

int main(){
	FreeImage_Initialise();

	FIBITMAP * bitmap = FreeImage_Allocate(WIDTH, HEIGHT, BPP);
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
	if( FreeImage_Save(FIF_PNG, bitmap, "complete_test_pixel.png", 0) )
		cout << "Image Successfully Saved!" << endl;
	FreeImage_DeInitialise();

	return 0;
}
