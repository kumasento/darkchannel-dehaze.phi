/*
 * WARNING: Make sure you've set all the info
 */ 


void hazy_pixels::pixelsSaveImageDarkChannelBitmap(){
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
}

void hazy_pixels::pixelsSaveImageRawOriginalBitmap(){
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
}

void hazy_pixels::pixelsSaveImageDarkChannelBitmap(){
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
}

void hazy_pixels::pixelsBuildtValueArray()
{
	int hei = this->hazy_height, wid = this->hazy_width;
	int arr_size = hei * wid;
	t_value_arr = (double) malloc(sizeof(double) * arr_size);
	double *gray_I_arr = (double) malloc(sizeof(double) * arr_size);

	//Pass 1: Set the array
	for(unsigned x = 0; x < hei; x++){
		for(unsigned y = 0; y < wid; y++){
			unsigned t_idx = DEF_XTtoIdx(x, y, wid);

			rgbtuple *tuple = pixelsGetRGBTupleByCoord(x, y);
			byte Ired = (byte) tuple->rgbred;
			byte Igreen = (byte) tuple->rgbgreen;
			byte Iblue = (byte) tuple->rgbblue;
			
			this->t_value_arr[t_idx] = (double)pixelsGetOriginaltValueByCoord(x, y);
			gray_I_arr[t_idx] = ((double) Ired + Igreen + Iblue) / 3;
		}
	}

	//Pass 2: Transfer it into a general_matrix
	general_matrix<double> t_value_mat(hei, wid, this->t_value_arr);
	general_matrix<double> gray_I_mat(hei, wid, gray_I_arr);

	general_matrix<double> q;
	int r = 20;
	double eps = 1e-3;
	guidedfilter(gray_I_mat, t_value_mat, r, eps, q);

	memcpy(this->t_value_arr, q.GetMatrixArray(), arr_size * sizeof(double));
}

void hazy_pixels::pixelsSaveImageMattedOriginalBitmap(){
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
			unsigned t_idx = DEF_XTtoIdx(x, y, this->hazy_width);
			double tValue = t_value_arr[t_idx];
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
}