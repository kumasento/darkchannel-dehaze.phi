#include <iostream>
#include <cstdio>

#include "Filter.h"

using namespace std;

int main(){
	int hei, wid, w; cin>>hei>>wid>>w;
	int *input_2d_arr = (int *) malloc(sizeof(int) * hei * wid);
	int *win_2d_arr = (int *) malloc(sizeof(int) * hei * wid);
	int *tran_2d_arr = (int *) malloc(sizeof(int) * hei * wid);

	for(int i = 0; i < hei*wid; i++)
		cin>>input_2d_arr[i];
	
	Filter::Compute2DMinWindowFilterArray(input_2d_arr, win_2d_arr, tran_2d_arr, hei, wid, w);
	
	for(int x = 0; x < hei; x ++){
		for(int y = 0; y < wid; y ++)
			cout << tran_2d_arr[y * hei + x] << " ";
		cout << endl;
	}
	
	return 0;
}
