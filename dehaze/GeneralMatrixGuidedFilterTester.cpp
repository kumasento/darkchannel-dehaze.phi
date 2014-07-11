#include <iostream>
#include "GeneralMatrix.h"
#include "GuidedImageFilter.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

using namespace std;

int main(){
	freopen("TestRes/guided_image_filter.in", "r", stdin);

	int hei, wid;
	while(cin >> hei >> wid){
		double *I_arr = (double *) malloc(sizeof(double) * hei * wid);
		for(int i = 0; i < hei * wid; i++)
			cin >> I_arr[i];
		double *p_arr = (double *) malloc(sizeof(double) * hei * wid);
		for(int i = 0; i < hei * wid; i++)
			cin >> p_arr[i];


		general_matrix<double> I(hei, wid, I_arr);
		general_matrix<double> p(hei, wid, p_arr);

		puts("I:"); I.PRINT();
		puts("p:"); p.PRINT();

		general_matrix<double> q;
		guidedfilter(I, p, 2, 0.1, q);
		
		puts("q:"); q.PRINT();
	}
}