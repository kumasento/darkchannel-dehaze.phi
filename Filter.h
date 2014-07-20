/**
 * All the functions in this library should not contain malloc or free.
 * For this library's running time should be counted
 */


#include <deque>

#define THREAD_NUM 240

typedef unsigned char f_byte;

namespace Filter{
	void ComputeMinRGBArray(f_byte* R_arr, f_byte* G_arr, f_byte* B_arr, f_byte* out_arr, int arr_size);

	template<typename T>
	void ComputeMinWindowFilterArray(T* input_arr, T* win_arr, int arr_size, int half_window_size);

	template<typename T>
	void Compute2DMinWindowFilterArray(T *input_2d_arr, T* win_2d_arr, T* tran_2d_arr, int hei, int wid, int hf_w_sz);
}

/**
 * ComputeMinRGBArray:
 * 
 * From 3 RGB array to get the min-value color channel for each pixels
 */
void Filter::ComputeMinRGBArray(f_byte* R_arr, f_byte* G_arr, f_byte* B_arr, f_byte* out_arr, int arr_size)
{
#ifdef __MIC__
#pragma omp parallel default(shared) num_threads(THREAD_NUM)
	{
		int id = omp_get_thread_num();
		int st_idx = id * arr_size / THREAD_NUM;

		for(int idx = st_idx; idx < arr_size && idx < (st_idx + arr_size / THREAD_NUM); idx ++){
			int R = R_arr[idx], G = G_arr[idx], B = B_arr[idx];
			int M = (R < G) ? R : G;
			M = (M < B) ? M : B;
			out_arr[idx] = M;
		}
	}

#else
	for(int idx = 0; idx < arr_size; idx ++){
		int R = R_arr[idx], G = G_arr[idx], B = B_arr[idx];
		int M = (R < G) ? R : G;
		M = (M < B) ? M : B;
		out_arr[idx] = M;
	}
#endif
}

template<typename T>
void Filter::ComputeMinWindowFilterArray(T* input_arr, T* value_arr, int arr_size, int half_window_size)
{
	std::deque<int> index_dq;

	index_dq.push_front(0);
	for(int i = 1; i <= half_window_size; i++){
		if(input_arr[i] < input_arr[i-1]){
			index_dq.pop_back();
			while(!index_dq.empty() && input_arr[index_dq.back()] > input_arr[i])
				index_dq.pop_back();
		}
		index_dq.push_back(i);
	}
	

	for(int i = half_window_size+1; i < arr_size; i++){
		int win_ub_idx = i - 1; 
		int win_md_idx = win_ub_idx - half_window_size;
		int win_lb_idx = win_md_idx - half_window_size;

		value_arr[win_md_idx] = input_arr[index_dq.front()];

		if(input_arr[i] < input_arr[i-1]){
			index_dq.pop_back();
			while(!index_dq.empty() && input_arr[index_dq.back()] > input_arr[i])
				index_dq.pop_back();
		}
		index_dq.push_back(i);
	
		if(index_dq.front() == win_lb_idx)
			index_dq.pop_front();
	}

	int arr_ub = arr_size + half_window_size;
	for(int i = arr_size; i <= arr_ub; i++){
		int win_ub_idx = i - 1; 
		int win_md_idx = win_ub_idx - half_window_size;
		int win_lb_idx = win_md_idx - half_window_size;
		
		value_arr[win_md_idx] = input_arr[index_dq.front()];

		if(index_dq.front() == win_lb_idx)
			index_dq.pop_front();
	}
}

template<typename T>
void Filter::Compute2DMinWindowFilterArray(T *input_2d_arr, T* win_2d_arr, T* tran_2d_arr, int hei, int wid, int hf_w_sz)
{
#ifdef __MIC__
#pragma omp parallel default(shared) num_threads(THREAD_NUM)
	{
		int id = omp_get_thread_num();
		int per_thr = hei / THREAD_NUM;

		for(int bias = per_thr * id * wid; bias < (per_thr * (id+1) * wid) && bias < hei*wid; bias += wid)
			Filter::ComputeMinWindowFilterArray(input_2d_arr+bias, win_2d_arr+bias, wid, hf_w_sz);
	}
#else
	for(int bias = 0; bias < hei * wid; bias += wid){
		Filter::ComputeMinWindowFilterArray(input_2d_arr+bias, win_2d_arr+bias, wid, hf_w_sz);
	}
#endif
	for(int y = 0; y < wid; y++){
		int x_bias = y * hei;
		for(int i = 0; i < hei; i++)
			input_2d_arr[x_bias+i] = win_2d_arr[i * wid + y];
		Filter::ComputeMinWindowFilterArray(input_2d_arr+x_bias, tran_2d_arr+x_bias, hei, hf_w_sz);
	}
}
