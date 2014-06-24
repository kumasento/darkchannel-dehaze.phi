#include <iostream>
#include "GeneralMatrix.h"
#include "GuidedImageFilter.h"
#include <cstdlib>

using namespace std;

int main(){
	freopen("TestRes/PrefixSumMatrix.in", "r", stdin);

	int height, width;
	while(cin >> height >> width){
		
		double *origin_arr = (double *) malloc(sizeof(double) * height * width);
		for(int i = 0; i < height * width; i++)
			cin >> origin_arr[i];
		general_matrix<double> Mat(height, width, origin_arr);

		printf("Mat Height: %d Width: %d\n", Mat.GetMatrixHeight(),
											 Mat.GetMatrixWidth());
		Mat.PrintMatrixArray();

		/* Here to test the district assignment function */
#ifdef TEST_DISTRICT_ASSIGN
		general_matrix<double> n_mat(2, 2, 1);
		Mat.DistrictAssignMatrix(3, 3, n_mat);
		Mat.PrintMatrixArray();
#endif

#ifdef TEST_EXTRACT_N_DISTRICT_ASSIGN		
		general_matrix<double> mat_cp(Mat);
		mat_cp.ExtractNDistrictAssignMatrix(0, 0, 3, 3, 2, 2, Mat);
		mat_cp.PrintMatrixArray();
#endif

#ifdef TEST_CAL_PREFIX_SUM		
		general_matrix<double> prefix_sum_mat;
		Mat.CalPrefixSum(prefix_sum_mat, 1);
		prefix_sum_mat.PrintMatrixArray();
#endif

#ifdef TEST_REP_MATRIX
		general_matrix<double> rep_mat;
		Mat.RepeatGenerateMatrix(rep_mat, 2, 2);
		rep_mat.PrintMatrixArray();
#endif

		general_matrix<double> imDst;
		boxfilter(Mat, imDst, 2);

		puts("BoxFilter Output!");
		imDst.PrintMatrixArray();

		
	}
	return 0;
}