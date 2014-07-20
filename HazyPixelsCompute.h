#include "GeneralMatrix.h"
#include "GuidedImageFilter.h"

#include <time.h>


int hazy_pixel_cmp(const void* a, const void *b){
	return (int)((hazy_pixel*)b)->dc_value - (int)((hazy_pixel*)a)->dc_value;
}

int hazy_pixel_inst_cmp(const void *a, const void *b){
	hazy_pixel *p1 = (hazy_pixel*) a;
	hazy_pixel *p2 = (hazy_pixel*) b;
	int p1_inst =(int)( 0.2126*p1->color_tuple->rgbred + 0.0722*p1->color_tuple->rgbblue + 0.7152*p1->color_tuple->rgbgreen);
	int p2_inst =(int)( 0.2126*p2->color_tuple->rgbred + 0.0722*p2->color_tuple->rgbblue + 0.7152*p2->color_tuple->rgbgreen);
	return p1_inst - p2_inst;
}

/**
 * pixelsBuildValueArray: 
 * use guided filter to change the t_value
 */
void hazy_pixels::pixelsBuildtValueArray(int r, double eps)
{
	unsigned hei = this->hazy_height, wid = this->hazy_width;
	int arr_size = hei * wid;
	this->t_value_arr = (double *) malloc(sizeof(double) * arr_size);
	
	/**
	 * Here I compute the guided image filter only use the gray image, because I'm lazy :<
	 */
	double *gray_I_arr = (double *) malloc(sizeof(double) * arr_size);

	//Pass 1: Set the array, both the [t_value_arr] and the [gray_I_arr]
	for(unsigned x = 0; x < hei; x++){
		for(unsigned y = 0; y < wid; y++){
			unsigned t_idx = DEF_XYtoIdx(x, y, wid);
			/*
			rgbtuple *tuple = pixelsGetRGBTupleByCoord(x, y);
			byte Ired = (byte) tuple->rgbred;
			byte Igreen = (byte) tuple->rgbgreen;
			byte Iblue = (byte) tuple->rgbblue;
			*/
			//For the OpenCV version, the hazy_pixel array has been totally built
			//So here I only need to access to the array

			byte Ired = (byte) this->pixel_array[t_idx].color_tuple->rgbred;
			byte Igreen = (byte) this->pixel_array[t_idx].color_tuple->rgbgreen;
			byte Iblue = (byte) this->pixel_array[t_idx].color_tuple->rgbblue;

			this->t_value_arr[t_idx] = (double)pixelsGetOriginaltValueByCoord(x, y);
			gray_I_arr[t_idx] = (double)(Ired * 30 + Igreen * 59 + Iblue * 11 + 50) / 100;
		}
	}

	//Pass 2: Transfer it into a general_matrix
	general_matrix<double> t_value_mat(hei, wid, this->t_value_arr);
	general_matrix<double> gray_I_mat(hei, wid, gray_I_arr);

	general_matrix<double> q;
	guidedfilter(gray_I_mat, t_value_mat, r, eps, q);

	memcpy(this->t_value_arr, q.GetMatrixArray(), arr_size * sizeof(double));
}

/**
 * pixelsGetOriginaltValueByCoord 
 * This one calculates the formula (12) in the paper
 */

double hazy_pixels::pixelsGetOriginaltValueByCoord(unsigned x, unsigned y){
	int idx  = x * this->hazy_width + y;
	double dc = this->pixel_array[idx].dc_value;
	double tValue = dc / this->A_value->rgbred;
	return 1 - 0.95*tValue;
}


void hazy_pixels::pixelsSetImageAtmosphereLightValue(){
	//0: Build a cp for pixel_array
	hazy_pixel * pixel_array_cp = (hazy_pixel *) malloc(sizeof(hazy_pixel) * 
														this->hazy_width *
														this->hazy_height);
	memcpy(pixel_array_cp, this->pixel_array, sizeof(hazy_pixel) * 
												this->hazy_width *
												this->hazy_height);


	//1: sort the pixel_array
	
	qsort(this->pixel_array, this->hazy_width *
							 this->hazy_height,
							 sizeof(this->pixel_array[0]),
							 hazy_pixel_cmp);

	unsigned array_upper_bound = (unsigned)(0.001 *
											(double) this->hazy_width * 
												   	 this->hazy_height);

	if(array_upper_bound == 0) array_upper_bound = DEF_MIN(10, 
													this->hazy_width *
													this->hazy_height);

	//2.1 Average
	/*
	this->A_value = (rgbtuple *) malloc(sizeof(rgbtuple));
	for(unsigned i = 0; i < array_upper_bound; i++){
		this->A_value->rgbred += this->pixel_array[i].color_tuple->rgbred;
		this->A_value->rgbgreen += this->pixel_array[i].color_tuple->rgbgreen;
		this->A_value->rgbblue += this->pixel_array[i].color_tuple->rgbblue;
	}

	this->A_value->rgbred = (int)((double)this->A_value->rgbred / array_upper_bound);
	this->A_value->rgbblue = (int)((double)this->A_value->rgbblue / array_upper_bound);
	this->A_value->rgbgreen = (int)((double)this->A_value->rgbgreen / array_upper_bound);
	*/
	//2.2 Max Intensity
	qsort(this->pixel_array, array_upper_bound,
							 sizeof(this->pixel_array[0]),
							 hazy_pixel_inst_cmp);

	this->A_value = (rgbtuple *) malloc(sizeof(rgbtuple));
	//2.2.1 Point:
	/*
	this->A_value->rgbred = this->pixel_array[0].color_tuple->rgbred;
	this->A_value->rgbblue = this->pixel_array[0].color_tuple->rgbblue;
	this->A_value->rgbgreen = this->pixel_array[0].color_tuple->rgbgreen;
	*/
	//2.2.2 Average:
	this->A_value->rgbred = this->pixel_array[0].color_tuple->rgbred;
	this->A_value->rgbblue = this->pixel_array[0].color_tuple->rgbblue;
	this->A_value->rgbgreen = this->pixel_array[0].color_tuple->rgbgreen;
	double av = 0.2126*A_value->rgbred +0.0722*A_value->rgbblue + 0.7152*A_value->rgbgreen;
	this->A_value->rgbred = av;
	this->A_value->rgbblue = av;
	this->A_value->rgbgreen = av;

	/*
	printf("A: [%u %u %u]\n",	this->A_value->rgbred,
								this->A_value->rgbgreen,
								this->A_value->rgbblue);
	*/
	memcpy(this->pixel_array, pixel_array_cp, sizeof(hazy_pixel) * 
												this->hazy_width *
												this->hazy_height);
	return ;
}

void hazy_pixels::pixelsSetResultMat(byte* R_arr, byte* G_arr, byte* B_arr){
	//Pass 1: Pre Calculate
	double scaledAvalue[3] = {(double) this->A_value->rgbred,
							  (double) this->A_value->rgbgreen,
							  (double) this->A_value->rgbblue};

	for(unsigned x = 0; x < this->hazy_height; x++){
		for(unsigned y = 0; y < this->hazy_width; y++){
			unsigned t_idx = DEF_XYtoIdx(x, y, this->hazy_width);
			double tValue = t_value_arr[t_idx];

			tValue = DEF_MAX(tValue, 0.1);
			
			rgbtuple * tuple = this->pixel_array[t_idx].color_tuple;
			
			byte Ired = (byte) tuple->rgbred;
			byte Igreen = (byte) tuple->rgbgreen;
			byte Iblue = (byte) tuple->rgbblue;

			//Change to [0,1]
			double scaledIvalue[3] = {(double) Ired,
									  (double) Igreen,
									  (double) Iblue};
			double scaledAnsValue[3] = {0.0, 0.0, 0.0};

			//Pass 1: Calculate Answer Vector
			for(int idx = 0; idx < 3; idx++){
				scaledAnsValue[idx] = (scaledIvalue[idx] - scaledAvalue[idx]) / 
									  tValue + 
									  scaledAvalue[idx];
				if(scaledAnsValue[idx] < 0) scaledAnsValue[idx] = 0;
			}
			//Pass 2: Scaled Down
			bool isOverflow = false; 
			for(int idx = 0; idx < 3; idx++)
				if(scaledAnsValue[idx] > 255) 
					isOverflow = true;
			if(isOverflow){
				/*
				printf("(%d %d) [%f %f %f]\n", x, y,
											 scaledAnsValue[0], 
											 scaledAnsValue[1],
											 scaledAnsValue[2]);
				*/
				double tmp_max = 0;
				for(int idx = 0; idx < 3; idx++)
					if(scaledAnsValue[idx] > tmp_max) 
						tmp_max = scaledAnsValue[idx];
				for(int idx = 0; idx < 3; idx++)
					scaledAnsValue[idx] = scaledAnsValue[idx]/tmp_max * 255;
			}

			R_arr[t_idx] =(byte) ((int)scaledAnsValue[0]);
			G_arr[t_idx] =(byte) ((int)scaledAnsValue[1]);
			B_arr[t_idx] =(byte) ((int)scaledAnsValue[2]);
		}
	}

	//this->CV_IMAGE_RES_MAT = saved_image_mat;
}

void hazy_pixels::pixelsCalculate(int r, double eps, byte* R_arr, byte* G_arr, byte* B_arr){

	std::cout << "INFO: Enter Computation" << std::endl;
	clock_t start_t, end_t;
	double duration;

	start_t = clock();
	this->pixelsSetDarkChannelValue();
	end_t = clock();
	duration = (double)(end_t-start_t) / CLOCKS_PER_SEC;
	printf("SetDarkChannelValue duration: %f sec\n", duration);

	start_t = clock();
	this->pixelsSetImageAtmosphereLightValue();
	end_t = clock();
	duration = (double)(end_t-start_t) / CLOCKS_PER_SEC;
	printf("SetImageAtmosphereLight duration: %f sec\n", duration);

	start_t = clock();
	this->pixelsBuildtValueArray(r, eps);
	end_t = clock();
	duration = (double)(end_t-start_t) / CLOCKS_PER_SEC;
	printf("SetBuildtValueArray duration: %f sec\n", duration);

	start_t = clock();
	this->pixelsSetResultMat(R_arr, G_arr, B_arr);
	end_t = clock();
	duration = (double)(end_t-start_t) / CLOCKS_PER_SEC;
	printf("SetResultMat duration: %f sec\n", duration);

}
