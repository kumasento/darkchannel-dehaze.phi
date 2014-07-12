
#include <vector>
#include <OpenCVInterfaces.h>

class hazy_pixels_video{
public:
	hazy_pixels_video();
	hazy_pixels_video(std::string video_name, int max_frame, int patch_size);

	void videoCalculate(int r, double eps);
	void videoSave();
private:
	std::vector<hazy_pixels> frames_array;
	std::vector<cv::Mat> CV_FRAMES_MAT;
	std::vector<cv::Mat> CV_FRAMES_RES_MAT;
	int frames_size;
};

hazy_pixels_video::hazy_pixels_video(std::string video_name, int max_frame, int patch_size){
	OpenCVInterfaces::CVVideo2Image(video_name.c_str(), CV_FRAMES_MAT, max_frame);

	this->frames_size = 0;
	std::vector<cv::Mat>::iterator frames_iter = CV_FRAMES_MAT.begin();
	while(frames_iter != CV_FRAMES_MAT.end()){
		//puts("New Frame");
		hazy_pixels tmp_hazy_pixels(*frames_iter);
		tmp_hazy_pixels.pixelsConfigure(1);
		tmp_hazy_pixels.pixelsSetImagePatchSize( patch_size );
		tmp_hazy_pixels.pixelsSetImagePixelArray();
		//puts("INFO: Finished setting the [pixel_array]");
		//std::cout << tmp_hazy_pixels.CV_IMAGE_MAT << std::endl;

		frames_array.push_back(tmp_hazy_pixels);
		frames_iter++;
		this->frames_size ++;
	}
	puts("INFO: Finished loading the video");
}

void hazy_pixels_video::videoCalculate(int r, double eps){
	std::vector<hazy_pixels>::iterator iter = frames_array.begin();
	while(iter != frames_array.end()){
		//std::cout << iter->CV_IMAGE_MAT << std::endl;
		iter->pixelsCalculate(r, eps);	
		CV_FRAMES_RES_MAT.push_back(iter->CV_IMAGE_RES_MAT);
		iter ++;
	}
}

void hazy_pixels_video::videoSave(){
	OpenCVInterfaces::CVImage2Video(this->CV_FRAMES_RES_MAT, this->frames_size);
	return;
}


