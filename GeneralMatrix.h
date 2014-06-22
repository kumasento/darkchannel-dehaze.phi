/*
 * This file provides the basic matrix operation interfaces
 * Created on 22/06/2014
 */

#define MAT_BIAS 10
#define MALLOC malloc
#define XYtoIdx(x, y, width) (((x) * (width)) + (y))

template <class Type>
class general_matrix{
public:
	general_matrix(){ this->mat_arr = NULL; };
	general_matrix(int height, int width){
		this->mat_height = height;
		this->mat_width = width;
		this->mat_size = height * width;
		this->mat_type_size = this->mat_size * sizeof(Type);
		this->mat_arr = (Type *) MALLOC(sizeof(Type) * (this->mat_size + MAT_BIAS));
		memset(this->mat_arr, 0, this->mat_type_size);
	}
	~general_matrix(){ 
		puts("WARNING: Destructor called!");
		if(this->mat_arr != NULL)
			free(this->mat_arr);
	}

	int GetMatrixHeight(){ return mat_height; }
	int GetMatrixWidth(){ return mat_width; }

	void ResizeMatrix(int height, int width){
		this->mat_height = height;
		this->mat_width = width;
		this->mat_size = height * width;
		this->mat_type_size = this->mat_size * sizeof(Type);
		if(this->mat_arr != NULL)
			free(this->mat_arr);

		this->mat_arr = (Type *) MALLOC(sizeof(Type) * (this->mat_size + MAT_BIAS));
		memset(this->mat_arr, 0, this->mat_type_size);
	};
	void CalPrefixSum(general_matrix<Type>& prefix_matrix, int model_type);
	void CopyMatrixArray(Type * tmp_arr, int tmp_size){ 
		memcpy(this->mat_arr, tmp_arr, tmp_size * sizeof(Type)); 
	}
	void PrintMatrixArray(){
		puts("Matrix:");
		for(int x = 0; x < this->mat_height; x++){
			for(int y = 0; y < this->mat_width; y++){
				int idx = XYtoIdx(x, y, this->mat_width);
				printf("%.4f ", mat_arr[idx]);
			}
			printf("\n");
		}
	}
private:
	int mat_height, mat_width;
	int mat_size;
	int mat_type_size;
	Type* mat_arr;
}; 

template <class Type>
void general_matrix<Type>::CalPrefixSum(general_matrix<Type>& prefix_matrix,int model_type){
	prefix_matrix.ResizeMatrix(this->mat_height, this->mat_width);
	Type *tmp_arr = (Type *) MALLOC(this->mat_type_size);
	memset(tmp_arr, 0, this->mat_type_size);
	
	/* Type 1 is in the row sequence */
	if(model_type == 1){
		for(int t_y = 0; t_y < this->mat_width; t_y ++){
			int idx = XYtoIdx(0, t_y, this->mat_width);
			tmp_arr[idx] = this->mat_arr[idx];
		}

		for(int t_x = 1; t_x < this->mat_height; t_x ++){
			for(int t_y = 0; t_y < this->mat_width; t_y ++){
				int idx = XYtoIdx(t_x, t_y, this->mat_width);
				tmp_arr[idx] = this->mat_arr[idx] + tmp_arr[idx - this->mat_width];
			}
		}

	}

	/* Type 2 is in column sequence */
	else if(model_type == 2){
		for(int t_x = 0; t_x < this->mat_height; t_x ++)
			tmp_arr[XYtoIdx(t_x, 0, this->mat_width)] = 
				this->mat_arr[XYtoIdx(t_x, 0, this->mat_width)];

		for(int t_y = 1; t_y < this->mat_width; t_y ++){
			for(int t_x = 0; t_x < this->mat_height; t_x ++){
				tmp_arr[XYtoIdx(t_x, t_y, this->mat_width)] = 
					this->mat_arr[XYtoIdx(t_x, t_y, this->mat_width)] + 
					tmp_arr[XYtoIdx(t_x, t_y-1, this->mat_width)];
			}
		}
	}

	prefix_matrix.CopyMatrixArray(tmp_arr, this->mat_size);
}
