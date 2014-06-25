/*
 * This file provides the basic matrix operation interfaces
 * Created on 22/06/2014
 */

#define MAT_BIAS 10
#define MALLOC malloc
#define FREE free
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
	general_matrix(int height, int width, Type start_val){
		this->mat_height = height;
		this->mat_width = width;
		this->mat_size = height * width;
		this->mat_type_size = this->mat_size * sizeof(Type);
		this->mat_arr = (Type *) MALLOC(sizeof(Type) * (this->mat_size + MAT_BIAS));
		for(int i = 0; i < this->mat_size; i++)
			this->mat_arr[i] = start_val;
	}
	general_matrix(general_matrix<Type>& tmp_general_matrix){
		this->mat_height = tmp_general_matrix.mat_height;
		this->mat_width = tmp_general_matrix.mat_width;
		this->mat_size = tmp_general_matrix.mat_size;
		this->mat_type_size = tmp_general_matrix.mat_type_size;
		this->mat_arr = (Type *) MALLOC(this->mat_type_size);
		memcpy(this->mat_arr, tmp_general_matrix.mat_arr, this->mat_type_size);
	}
	general_matrix(int height, int width, Type *array){
		this->mat_height = height;
		this->mat_width = width;
		this->mat_size = height * width;
		this->mat_type_size = this->mat_size * sizeof(Type);
		this->mat_arr = (Type *) MALLOC(sizeof(Type) * (this->mat_size + MAT_BIAS));
		memcpy(this->mat_arr, array, this->mat_type_size);
	}
	~general_matrix(){ 
		//puts("WARNING: Destructor called!");
		if(this->mat_arr != NULL)
			free(this->mat_arr);
	}

	int GetMatrixHeight(){ return mat_height; }
	int GetMatrixWidth(){ return mat_width; }
	Type * GetMatrixArray(){ return mat_arr; }

	/* Basic Matrix Handlers */
	void ResizeMatrix(int height, int width, Type start_val){
		this->mat_height = height;
		this->mat_width = width;
		this->mat_size = height * width;
		this->mat_type_size = this->mat_size * sizeof(Type);
		if(this->mat_arr != NULL)
			free(this->mat_arr);

		this->mat_arr = (Type *) MALLOC(sizeof(Type) * (this->mat_size + MAT_BIAS));
		for(int i = 0; i < this->mat_size; i++)
			this->mat_arr[i] = start_val;
	};
	void CopyMatrixArray(Type * tmp_arr, int tmp_size){ 
		memcpy(this->mat_arr, tmp_arr, tmp_size * sizeof(Type)); 
	}
	int DistrictAssignMatrix(int x, int y, general_matrix<Type>& mat)
	{
		if( x + mat.mat_height > this->mat_height ||
			y + mat.mat_width > this->mat_width )
		{
			puts("ERROR: Wrong Distance");
			return 0;
		}
		for(int dx = 0; dx < mat.mat_height; dx ++){
			for(int dy = 0; dy < mat.mat_width; dy ++){
				int idx = XYtoIdx((x+dx), (y+dy), this->mat_width);
				int t_idx = XYtoIdx(dx, dy, mat.mat_width);
				this->mat_arr[idx] = mat.mat_arr[t_idx];
			}
		}
		return 1;
	}
	int RepeatGenerateMatrix(general_matrix<Type>& dst_mat, int rep_x, int rep_y);
	void PrintMatrixArray(){
		puts("Matrix:");
		if(this->mat_arr == NULL){
			puts("ERROR: This is an empty matrix!");
			return ;
		}
		else{
			printf("Height: %d Width: %d\n", this->mat_height, this->mat_width);
			/*
			for(int x = 0; x < this->mat_height; x++){
				for(int y = 0; y < this->mat_width; y++){
					int idx = XYtoIdx(x, y, this->mat_width);
					printf("%10.4f ", mat_arr[idx]);
				}
				printf("\n");
			}
			*/
		}
	}

	/* Basic Matrix Operations */
	general_matrix<Type>& operator =(general_matrix<Type> &tmp_general_matrix)
	{
		this->mat_height = tmp_general_matrix.mat_height;
		this->mat_width = tmp_general_matrix.mat_width;
		this->mat_size = tmp_general_matrix.mat_size;
		this->mat_type_size = tmp_general_matrix.mat_type_size;

		if(this->mat_arr != NULL)
			free(this->mat_arr);

		this->mat_arr = (Type *) MALLOC(this->mat_type_size);
		memcpy(this->mat_arr, tmp_general_matrix.mat_arr, this->mat_type_size);

		return *this;
	}
	general_matrix<Type>& operator -(general_matrix<Type> &mat)
	{
		for(int i = 0; i < this->mat_size; i++)
			this->mat_arr[i] -= mat.mat_arr[i];
		return *this;
	}
	general_matrix<Type>& operator +(general_matrix<Type> &mat)
	{
		for(int i = 0; i < this->mat_size; i++)
			this->mat_arr[i] += mat.mat_arr[i];
		return *this;
	}
	general_matrix<Type>& operator -=(general_matrix<Type>& mat){
		for(int i = 0; i < this->mat_size; i++)
			this->mat_arr[i] -= mat.mat_arr[i];
		return *this;
	}
	int ArrayDotOptFilter(Type * src_mat1, Type * src_mat2, char opt);
	int MatrixOptFilter(general_matrix<Type>& src_mat1, general_matrix<Type>& src_mat2, char opt);

	int ExtractMatrix(int x, int dx, int y, int dy, general_matrix<Type>& gen_mat);
	Type * ExtractMatrixToArray(int x, int dx, int y, int dy);
	int ExtractNDistrictAssignMatrix( int dst_x, int dst_y, 
									  int src_x, int src_y,
									  int dx, int dy, 
									  general_matrix<Type>& src_mat);

	/* High Level */
	void CalPrefixSum(general_matrix<Type>& prefix_matrix, int model_type);

private:
	int mat_height, mat_width;
	int mat_size;
	int mat_type_size;
	Type* mat_arr;
}; 

template <class Type>
int general_matrix<Type>::ArrayDotOptFilter(Type * src_mat1, Type * src_mat2, char opt)
{
	switch(opt){
		case '*':
			for(int i = 0; i < this->mat_size; i++)
				this->mat_arr[i] = src_mat1[i] * src_mat2[i];
			break;
		case '/':
			for(int i = 0; i < this->mat_size; i++)
				this->mat_arr[i] = src_mat1[i] / src_mat2[i];
			break;
		case '+':
			for(int i = 0; i < this->mat_size; i++)
				this->mat_arr[i] = src_mat1[i] + src_mat2[i];
			break;
		case '-':
			for(int i = 0; i < this->mat_size; i++)
				this->mat_arr[i] = src_mat1[i] - src_mat2[i];
			break;
		default:
			break;
	}
	return 1;
}

template <class Type>
int general_matrix<Type>::MatrixOptFilter(general_matrix<Type>& src_mat1, 
										  general_matrix<Type>& src_mat2, 
										  char opt)
{
	int src_hei1 = src_mat1.GetMatrixHeight(), src_hei2 = src_mat2.GetMatrixHeight();
	int src_wid1 = src_mat1.GetMatrixWidth(), src_wid2 = src_mat2.GetMatrixWidth();

	if(src_hei1 != src_hei2 || src_wid1 != src_wid2){
		puts("ERROR: The 2 matrices don't match!");
		return 0;
	}
	Type * src_arr1 = src_mat1.GetMatrixArray();
	Type * src_arr2 = src_mat2.GetMatrixArray();
	
	this->ResizeMatrix( src_hei1, src_wid1, 0 );
	this->ArrayDotOptFilter( src_arr1, src_arr2, opt );

	return 1;
}

template <class Type>
int general_matrix<Type>::RepeatGenerateMatrix(general_matrix<Type>& dst_mat, int rep_x, int rep_y)
{
	int dst_height = rep_x * this->mat_height;
	int dst_width = rep_y * this->mat_width;
	int dst_size = dst_height * dst_width;
	//printf("%d %d\n", dst_height, dst_width);

	Type *dst_arr = (Type*) MALLOC(dst_size * sizeof(Type));
	for(int rep_tx = 0; rep_tx < rep_x; rep_tx ++){
		for(int rep_ty = 0; rep_ty < rep_y; rep_ty ++){
			int start_x = rep_tx * this->mat_height;
			int start_y = rep_ty * this->mat_width;
			//printf("%d %d\n", start_x, start_y);
			for(int tx = 0; tx < this->mat_height; tx ++){
				for(int ty = 0; ty < this->mat_width; ty ++){
					int src_idx = XYtoIdx(tx, ty, this->mat_width);
					int dst_idx = XYtoIdx((start_x+tx), (start_y+ty), dst_width);
					//printf("%d ", dst_idx);
					dst_arr[dst_idx] = this->mat_arr[src_idx];
				}
			}
		}
	}

	/*
	for(int i = 0; i < dst_size; i++)
		printf("%f ", dst_arr[i]);
	*/

	dst_mat.ResizeMatrix(dst_height, dst_width, 0);
	dst_mat.CopyMatrixArray(dst_arr, dst_size);
	return 1;
}

template <class Type>
int general_matrix<Type>::ExtractMatrix(int x, int dx, int y, int dy, general_matrix<Type>& gen_mat)
{
	if(dx <= 0 || dy <= 0){
		puts("ERROR: Wrong Parameters");
		return 0;
	}
	gen_mat.ResizeMatrix(dx, dy, 0);

	Type *tmp_arr = (Type *) MALLOC(sizeof(Type) * dx * dy);
	for(int tdx = 0; tdx < dx; tdx ++){
		for(int tdy = 0; tdy < dy; tdy ++){
			int idx = XYtoIdx((x+tdx), (y+tdy), this->mat_width);
			int t_idx = XYtoIdx(tdx, tdy, dy);
			tmp_arr[t_idx] = this->mat_arr[idx];
		}
	}

	gen_mat.CopyMatrixArray(tmp_arr, dx * dy);

	FREE(tmp_arr);
	return 1;
}

template <class Type> 
Type * general_matrix<Type>::ExtractMatrixToArray(int x, int dx, int y, int dy)
{
	Type *tmp_arr = (Type *) MALLOC(sizeof(Type) * dx * dy);
	for(int tdx = 0; tdx < dx; tdx ++){
		for(int tdy = 0; tdy < dy; tdy ++){
			int idx = XYtoIdx((x+tdx), (y+tdy), this->mat_width);
			int t_idx = XYtoIdx(tdx, tdy, dy);
			tmp_arr[t_idx] = this->mat_arr[idx];
		}
	}
	return tmp_arr;
}

template <class Type>
int general_matrix<Type>::ExtractNDistrictAssignMatrix( int dst_x, int dst_y, 
								  						int src_x, int src_y,
								  						int dx, int dy, 
								  						general_matrix<Type>& src_mat)
{
	// Without Error Check, pls be careful about this!
	Type *ext_arr = src_mat.ExtractMatrixToArray(src_x, dx, src_y, dy);
	for(int tdx = 0; tdx < dx; tdx ++){
		for(int tdy = 0; tdy < dy; tdy ++){
			int dst_idx = XYtoIdx((dst_x+tdx), (dst_y+tdy), this->mat_width);
			int src_idx = XYtoIdx(tdx, tdy, dy);
			this->mat_arr[dst_idx] = ext_arr[src_idx];
		}
	}
	FREE(ext_arr);
	return 1;
}


template <class Type>
void general_matrix<Type>::CalPrefixSum(general_matrix<Type>& prefix_matrix,int model_type)
{
	prefix_matrix.ResizeMatrix(this->mat_height, this->mat_width, 0);
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

