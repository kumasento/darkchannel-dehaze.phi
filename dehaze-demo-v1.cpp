#include <iostream>
#include <cstdio>
#include <unistd.h>

#include <HazyPixels.h>
#include <HazyPixelsSave.h>
#include <HazyPixelsCompute.h>
#include <HazyPixelsVideo.h>

using namespace std;

int main(int argc, char *argv[]){
	int opt;
	string file_name;
	bool show_info = false;
	int patch_size = 3;
	int r = 30;
	double eps = 1e-3;
	int file_type = 1;
	int frames_max = 20;
	int gvar_dc_type = 1;

	while((opt = getopt(argc, argv, "f:iw:r:e:t:g:h:d:")) != -1){
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
			case 'r':
				r = atoi(optarg);
				break;
			case 'e':
			{
				int times = atoi(optarg); eps = 1;
				for(int i = 0; i < times; i ++)
					eps /= 10;
				break;
			}
			case 't':
				file_type = atoi(optarg);
				break;
			case 'h':
				cout << "Usage: -f [file_name] -t [file_type: 1-image/ 2-video]" << endl;
				break;
			case 'g':
				frames_max = atoi(optarg);
				break;
			case 'd':
				gvar_dc_type = atoi(optarg);
				break;
			default:
				return 1;
		}
	}
	if(file_type == IMAGE_TYPE){
		cout << "INFO: Image Type" << endl;
		cout << "INFO: Start" << endl;
		hazy_pixels hpixels(file_name.c_str());
	
		hpixels.pixelsConfigure(gvar_dc_type);
		hpixels.pixelsSetImagePatchSize( patch_size );
		hpixels.pixelsPrintImageInfo();
		hpixels.pixelsSetImagePixelArray();
		puts("INFO: Finished setting the [pixel_array]\n");
		
		clock_t start = clock();
		hpixels.pixelsCalculate(r, eps);
		clock_t end = clock();
		double duration = (double)(end-start) / CLOCKS_PER_SEC;
		printf("1-time duration: %f sec\n", duration);
		printf("1-time per Mp result: %f sec\n", duration / (hpixels.hazy_width * hpixels.hazy_height / 1e6));
		OpenCVInterfaces::CVImageSaver("res.png", hpixels.CV_IMAGE_RES_MAT);
		OpenCVInterfaces::CVImageShower(hpixels.CV_IMAGE_MAT ,hpixels.CV_IMAGE_RES_MAT);

		hpixels.pixelsUnLoader();
		cout << "INFO: UnLoaded" << endl;
	}
	else if(file_type == VIDEO_TYPE){
		cout << "Video Name: " << file_name << endl;
		cout << "Frame Max Number: " << frames_max << endl;

		clock_t start = clock();
		hazy_pixels_video hpvideo(file_name, frames_max, patch_size);
		clock_t end = clock();
		double duration = (double)(end-start) / CLOCKS_PER_SEC;
		printf("loading duration per frame: %f sec\n", duration / frames_max);

		start = clock();
		hpvideo.videoCalculate(r, eps);
		end = clock();
		duration = (double)(end-start) / CLOCKS_PER_SEC;
		printf("calculation duration per frame: %f sec\n", duration/frames_max);

		hpvideo.videoSave();
	}
	return 0;
}
