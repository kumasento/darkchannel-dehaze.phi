#include <iostream>
#include <cstdio>

#include "Filter.h"

using namespace std;

int main(){
	int N, w; cin>>N>>w;
	int *input_arr, *value_arr;
	input_arr = (int *) malloc(sizeof(int) * N);
	value_arr = (int *) malloc(sizeof(int) * N);

	for(int i = 0; i < N; i++)
		cin>>input_arr[i];

	Filter::ComputeMinWindowFilterArray(input_arr, value_arr, N, w);

	for(int i = 0; i < N; i++)
		printf("%d ", value_arr[i]);
	printf("\n");

	return 0;
}
