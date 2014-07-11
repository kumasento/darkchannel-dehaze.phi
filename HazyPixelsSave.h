/*
 * WARNING: Make sure you've set all the info
 */ 


/**
 * pixelsSaveOriginalImage:
 * Save the image based on hazy_pixels data structure
 *
 * You could use this one like this:
 * hazy_pixels hp;
 * hp.pixelsSaveOriginalImage;
 */

void hazy_pixels::pixelsSaveOriginalImage(const char *file_name){
	std::string saved_file_name = std::string(file_name) + std::string("_original.png");

	cv::Mat saved_image_mat(this->hazy_height, this->hazy_width,  CV_8UC3);
	int Idx = 0;

	for(unsigned i = 0; i < saved_image_mat.rows; i++){
		for(unsigned j = 0; j < saved_image_mat.cols; j++){
			/*
			printf("(%d %d): ", i, j);
			printf("(R %3d G %3d B %3d)\n", this->pixel_array[Idx].color_tuple->rgbred,
											this->pixel_array[Idx].color_tuple->rgbgreen, 
											this->pixel_array[Idx].color_tuple->rgbblue);
			*/
			saved_image_mat.at<cv::Vec3b>(i,j)[0] = this->pixel_array[Idx].color_tuple->rgbblue;
			saved_image_mat.at<cv::Vec3b>(i,j)[1] = this->pixel_array[Idx].color_tuple->rgbgreen;
			saved_image_mat.at<cv::Vec3b>(i,j)[2] = this->pixel_array[Idx].color_tuple->rgbred;
			Idx++;
		}
	}

	//std::cout << saved_image_mat << std::endl;	
	OpenCVInterfaces::CVImageSaver(saved_file_name.c_str(), saved_image_mat);
}



void hazy_pixels::pixelsSaveImageRawOriginalBitmap(){
#ifdef FREE_IMAGE_SUPPORT
	char out_file_name[60]; memset(out_file_name, 0, sizeof(out_file_name));
	sprintf(out_file_name, "%s_transferred_%u.png", this->hazy_file_name,
													this->hazy_patch_size);
	
	FIBITMAP *orBitmap = FIInterfaceGenerateBitmapColorBits(this->hazy_width,
															this->hazy_height);
	double *scaledAnsValueArray = (double*) malloc( sizeof(double) *
													this->hazy_width *
													this->hazy_height *
													3);
	
	this->pixelsSetImageAtmosphereLightValue();

	double tmp_Max[3] = { 0.0, 0.0, 0.0 };
	for(unsigned x = 0; x < this->hazy_width; x++){
		for(unsigned y = 0; y < this->hazy_height; y++){
			double tValue = (double)pixelsGetOriginaltValueByCoord(x, y);
			tValue = DEF_MAX(tValue, 0.1);

			rgbtuple *tuple = pixelsGetRGBTupleByCoord(x, y);
			byte Ired = (byte) tuple->rgbred;
			byte Igreen = (byte) tuple->rgbgreen;
			byte Iblue = (byte) tuple->rgbblue;

			//Change to [0,1]
			double scaledAvalue[3] = {(double) this->A_value->rgbred,
									  (double) this->A_value->rgbgreen,
									  (double) this->A_value->rgbblue};
			double scaledIvalue[3] = {(double) Ired,
									  (double) Igreen,
									  (double) Iblue};
			double scaledAnsValue[3] = {0.0, 0.0, 0.0};
			
			for(int idx = 0; idx < 3; idx++){
				scaledAnsValue[idx] = (scaledIvalue[idx] - scaledAvalue[idx]) / 
									  tValue + 
									  scaledAvalue[idx];
				if(scaledAnsValue[idx] > tmp_Max[idx])
					tmp_Max[idx] = scaledAnsValue[idx];
			}

			unsigned ArrayIdx = DEF_XYZtoIdx(x, y, 0, this->hazy_height, 3); 

			scaledAnsValueArray[ArrayIdx] = scaledAnsValue[0];
			scaledAnsValueArray[ArrayIdx + 1] = scaledAnsValue[1];
			scaledAnsValueArray[ArrayIdx + 2] = scaledAnsValue[2];
		}
	}
	for(unsigned x = 0; x < this->hazy_width; x++){
		for(unsigned y = 0; y < this->hazy_height; y++){

			RGBQUAD *rgbRes = (RGBQUAD*) malloc(sizeof(RGBQUAD));

			unsigned ArrayIdx = DEF_XYZtoIdx(x, y, 0, this->hazy_height, 3);
			for(int idx = 0; idx < 3; idx++){
				scaledAnsValueArray[ArrayIdx + idx] /= tmp_Max[idx];	
			}
			//printf("%f %f %f\n", scaledAnsValue[0], scaledAnsValue[1], scaledAnsValue[2]);
			/*	
			printf("[%u %u %u]\n", (unsigned)(scaledAnsValue[0] * 255),
								   (unsigned)(scaledAnsValue[1] * 255),
								   (unsigned)(scaledAnsValue[2] * 255));
			*/	

			rgbRes->rgbRed = (byte) ( scaledAnsValueArray[ArrayIdx] * 255 );
			rgbRes->rgbGreen = (byte) ( scaledAnsValueArray[ArrayIdx + 1] * 255);
			rgbRes->rgbBlue = (byte) ( scaledAnsValueArray[ArrayIdx + 2] * 255);

			FreeImage_SetPixelColor(orBitmap,
									x,
									y,
									rgbRes);
		}
	}

	FreeImage_Save(FIF_PNG, orBitmap, out_file_name, 0);
#endif
}

void hazy_pixels::pixelsSaveImageDarkChannelBitmap(){
#ifdef FREE_IMAGE_SUPPORT
	char file_name[60]; memset(file_name, 0, sizeof(file_name));
	sprintf(file_name, "dark_channel_bitmap_%u.png", this->hazy_patch_size);
	FIBITMAP *dcBitmap = FIInterfaceGenerateBitmapEightBits(this->hazy_width,
															this->hazy_height);

	for(unsigned x = 0; x < this->hazy_width; x++){
		for(unsigned y = 0; y < this->hazy_height; y++){
			byte dcValue = pixelsGetDarkChannelByCoord(x, y);
			//printf("(%u %u) %u ", x, y, dcValue);
			FreeImage_SetPixelIndex(dcBitmap,
									x,
									y,
									&dcValue);
		}
	}

	FreeImage_Save(FIF_PNG, dcBitmap, file_name, 0);
#endif
}


void hazy_pixels::pixelsSaveImageMattedOriginalBitmap(int r = 30, double eps = 1e-3){
	char out_file_name[60]; memset(out_file_name, 0, sizeof(out_file_name));
	sprintf(out_file_name, "%s_2a_transferred_%u_%u.png", 
													this->hazy_file_name,
													this->hazy_patch_size,
													r);
	
	this->pixelsCalculate(r, eps);

	OpenCVInterfaces::CVImageSaver(out_file_name, this->CV_IMAGE_RES_MAT);
	OpenCVInterfaces::CVImageShower(this->CV_IMAGE_RES_MAT);
}
