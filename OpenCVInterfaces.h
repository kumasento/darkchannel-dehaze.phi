#ifdef __APPLE__
	#include <opencv/cv.h>	
	#include <opencv/highgui.h>
#endif


#define VIDEO_TYPE 2
#define IMAGE_TYPE 1

typedef uchar elem_t;

namespace OpenCVInterfaces
{	
	bool CVImageLoader(const char * file_name, cv::Mat& CV_IMAGE_MAT);
	void CVImagePicChecker(const char *file_name);
	void CVImageSaver(const char *file_name, cv::Mat& CV_IMAGE_MAT);
}

bool OpenCVInterfaces::CVImageLoader(const char *file_name, cv::Mat& CV_IMAGE_MAT){
	CV_IMAGE_MAT = cv::imread(file_name, CV_LOAD_IMAGE_COLOR);
	if(!CV_IMAGE_MAT.data){
		printf("OPENCV_ERROR: No Image Data!\n");
		return false;
	}
	return true;
}
void OpenCVInterfaces::CVImagePicChecker(const char *file_name){
	cv::Mat CV_IMAGE_MAT;
	OpenCVInterfaces::CVImageLoader(file_name, CV_IMAGE_MAT);

	int CV_IMAGE_CHANNELS = CV_IMAGE_MAT.channels();
	int CV_IMAGE_ROWS = CV_IMAGE_MAT.rows;
	int CV_IMAGE_COLS = CV_IMAGE_MAT.cols * CV_IMAGE_CHANNELS;

	printf("channel number: %d\n", CV_IMAGE_CHANNELS);
	printf("rows: %d cols: %d\n", CV_IMAGE_ROWS, CV_IMAGE_COLS);
	printf("continuous: %d\n", CV_IMAGE_MAT.isContinuous());

	if(CV_IMAGE_MAT.isContinuous()){
		CV_IMAGE_COLS *= CV_IMAGE_ROWS;
		CV_IMAGE_ROWS = 1; 
	}

	printf("depth: %u CV_8U: %d\n", CV_IMAGE_MAT.depth(), CV_8U);

	elem_t* p = CV_IMAGE_MAT.data;
	for(int i = 0; i < CV_IMAGE_COLS * CV_IMAGE_ROWS; i++){
		if(i % 3 == 0) printf("B: %5u ", p[i]);
		else if(i % 3 == 1) printf("R: %5u ", p[i]);
		else if(i % 3 == 2) printf("G: %5u\n", p[i]);
	}
	return;
}

void OpenCVInterfaces::CVImageSaver(const char *file_name, cv::Mat& CV_IMAGE_MAT){
	cv::imwrite(file_name, CV_IMAGE_MAT);
	printf("INFO: Image \"%s\" saved", file_name);
	return ;
}

