#include <iostream>
#include <GeneralMatrix.h>
#include <cstdlib>

using namespace std;

int main(){
	freopen("TestRes/PrefixSumMatrix.in", "r", stdin);

	int height, width;
	while(cin >> height >> width){
		general_matrix<double> Mat(height, width);
		
		printf("Mat Height: %d Width: %d\n", Mat.GetMatrixHeight(),
											 Mat.GetMatrixWidth());
		double *origin_arr = (double *) malloc(sizeof(double) * height * width);
		for(int i = 0; i < height * width; i++)
			cin >> origin_arr[i];

		Mat.CopyMatrixArray(origin_arr, height * width);
		Mat.PrintMatrixArray();

		for(int i = 1; i <= 2; i++){
			printf("Test Prefix Sum: type-%d\n", i);

			general_matrix<double> prefix_matrix;
			Mat.CalPrefixSum(prefix_matrix, i);
			prefix_matrix.PrintMatrixArray();
		}
	}
	return 0;
}