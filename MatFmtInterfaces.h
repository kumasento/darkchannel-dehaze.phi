/*
 * This header file contains the file I/O operations
 */

#include <fstream>

namespace MatFmt{
	bool MatFmtHWLdr(int& h, int &w, const char *mat_fl_n){
		std::ifstream in_fl(mat_fl_n);
		in_fl >> h >> w;
		return true;
	}
	template<typename T>
	bool MatFmtRGBLdr(T* R_arr, T* G_arr, T* B_arr, int& h, int& w, const char *mat_fl_n)
	{
		std::ifstream in_fl(mat_fl_n);
		in_fl >> h >> w;
		int sz = h * w;
		for(int idx = 0; idx < sz; idx++){
			int in_n; in_fl >> in_n;
			R_arr[idx] = (T) in_n;
		}
		for(int idx = 0; idx < sz; idx++){
			int in_n; in_fl >> in_n;
			G_arr[idx] = (T) in_n;
		}
		for(int idx = 0; idx < sz; idx++){
			int in_n; in_fl >> in_n;
			B_arr[idx] = (T) in_n;
		}
		
		return true;
	}
	template<typename T>
	bool MatFmtRGBWtr(T* R_arr, T* G_arr, T* B_arr, int h, int w, const char *mat_fl_n)
	{
		std::ofstream out_fl(mat_fl_n);
		out_fl << h << ' ' << w << std::endl;
		int sz = h * w;

		for(int idx = 0; idx < sz; idx ++)
			out_fl << (int)R_arr[idx] << ' ';
		out_fl << std::endl;
		for(int idx = 0; idx < sz; idx ++)
			out_fl << (int)G_arr[idx] << ' ';
		out_fl << std::endl;
		for(int idx = 0; idx < sz; idx ++)
			out_fl << (int)B_arr[idx] << ' ';
		out_fl << std::endl;

		return true;
	}
}
