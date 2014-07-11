#ifndef _DEF_OPENCV_INTERFACES_

#define _DEF_OPENCV_INTERFACES_

#ifdef __APPLE__
#include <opencv/cv.h>	
#include <opencv/highgui.h>
#else
#include <opencv/cv.h>
#include <opencv/highgui.h>
#endif

#include <vector>

#define VIDEO_TYPE 2
#define IMAGE_TYPE 1

typedef uchar elem_t;

namespace OpenCVInterfaces
{	
	bool CVImageLoader(const char * file_name, cv::Mat& CV_IMAGE_MAT);
	void CVImagePicChecker(const char *file_name);
	void CVImageSaver(const char *file_name, cv::Mat& CV_IMAGE_MAT);
	void CVImageShower(cv::Mat& CV_IMAGE_MAT);
	bool CVVideo2Image(const char * file_name, std::vector<cv::Mat>& CV_FRAMES_MAT, int max_frames );
	bool CVImage2Video(std::vector<cv::Mat>& CV_FRAMES_RES_MAT, int max_frames);
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

void OpenCVInterfaces::CVImageShower(cv::Mat& CV_IMAGE_MAT){
	cv::namedWindow("Display", cv::WINDOW_AUTOSIZE );
	cv::imshow("Display", CV_IMAGE_MAT);
	cv::waitKey(0);
	return ;
}

bool OpenCVInterfaces::CVVideo2Image(const char * file_name, std::vector<cv::Mat>& CV_FRAMES_MAT, int max_frames ){
	std::string video_name(file_name);
	cv::VideoCapture captured_video(video_name);

	if(!captured_video.isOpened()){
		printf("ERROR: Failed to open a video!\n");
		return false;
	}

	for(int i = 0; i < max_frames; i++){
		cv::Mat captured_frame;
		captured_video.read(captured_frame);
		if(!captured_frame.empty())
			CV_FRAMES_MAT.push_back(captured_frame);
		//std::cout << captured_frame << std::endl;
	}
	captured_video.release();
	return true;
}

bool OpenCVInterfaces::CVImage2Video(std::vector<cv::Mat>& CV_FRAMES_RES_MAT, int max_frames){
	int numframes = max_frames; 
    int fourcc    = CV_FOURCC('j','p','e','g');   
    double fps    = 20;  
    int frameWidth = CV_FRAMES_RES_MAT[0].cols; 
    int frameHeight = CV_FRAMES_RES_MAT[0].rows; 
    cv::VideoWriter Writer;  

    std::string _videoTosave("out.mov");
    Writer = cv::VideoWriter(_videoTosave,fourcc,fps,cv::Size(frameWidth,frameHeight), 1);  
    for (int i = 0; i < numframes; i ++){  
        Writer.write(CV_FRAMES_RES_MAT[i]);
        //OpenCVInterfaces::CVImageShower(CV_FRAMES_RES_MAT[i]);  
    }  

    return true;
}

#endif

