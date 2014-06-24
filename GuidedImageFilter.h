/*
 * This is based on the original MATLAB version from the author
 * Created on 06/22/2014
 */

//#define PRESENT_MATLAB_LINE_NUM
#define PRINT PrintMatrixArray

template <class value_t>
int boxfilter(general_matrix<value_t>& imSrc, general_matrix<value_t>& imDst, int r)
{
	/*
	 *	MATLAB Code:
	 *	[hei, wid] = size(imSrc);
	 *	imDst = zeros(size(imSrc));
	 */

	int hei = imSrc.GetMatrixHeight(), wid = imSrc.GetMatrixWidth();
	imDst.ResizeMatrix(hei, wid, 0);

	/*
	 *	MATLAB Code:
	 *	%cumulative sum over Y axis
	 *	imCum = cumsum(imSrc, 1); 
	 */
	general_matrix<value_t> imCum;
	imSrc.CalPrefixSum(imCum, 1);

	/*
	MATLAB Code:
	%difference over Y axis
	1 imDst(1:r+1, :) = imCum(1+r:2*r+1, :);
	2 imDst(r+2:hei-r, :) = imCum(2*r+2:hei, :) - imCum(1:hei-2*r-1, :);
	3 imDst(hei-r+1:hei, :) = repmat(imCum(hei, :), [r, 1]) - imCum(hei-2*r:hei-r-1, :);

	Here I use 2 temp variables to apply the algorithm
	*/
	//line 1
	imDst.ExtractNDistrictAssignMatrix(0, 0, r, 0, r+1, wid, imCum);
	//line2
	general_matrix<value_t> tmp_mat;
	imCum.ExtractMatrix(2*r+1, hei-2*r-1, 0, wid, tmp_mat);
	general_matrix<value_t> tmp_mat2;
	imCum.ExtractMatrix(0, hei-2*r-1, 0, wid, tmp_mat2);
	tmp_mat -= tmp_mat2;
	imDst.DistrictAssignMatrix(r+1, 0, tmp_mat);
	//line 3
	imCum.ExtractMatrix(hei-1, 1, 0, wid, tmp_mat);
	tmp_mat.RepeatGenerateMatrix(tmp_mat2, r, 1);
	imCum.ExtractMatrix(hei-2*r-1, r, 0, wid, tmp_mat);
	tmp_mat = tmp_mat2 - tmp_mat;
	imDst.DistrictAssignMatrix(hei-r, 0, tmp_mat);
	/*
	MATLAB Code:
	%cumulative sum over X axis
	imCum = cumsum(imDst, 2);
	*/
	imDst.CalPrefixSum(imCum, 2);

	/*
	MATLAB Code:
	%difference over Y axis
	imDst(:, 1:r+1) = imCum(:, 1+r:2*r+1);
	imDst(:, r+2:wid-r) = imCum(:, 2*r+2:wid) - imCum(:, 1:wid-2*r-1);
	imDst(:, wid-r+1:wid) = repmat(imCum(:, wid), [1, r]) - imCum(:, wid-2*r:wid-r-1);
	*/
	//line 1
	imDst.ExtractNDistrictAssignMatrix(0, 0, 0, r, hei, r+1, imCum);
	//line2
	imCum.ExtractMatrix(0, hei, 2*r+1, wid-2*r-1, tmp_mat);
	imCum.ExtractMatrix(0, hei, 0, wid-2*r-1, tmp_mat2);
	tmp_mat -= tmp_mat2;
	imDst.DistrictAssignMatrix(0, r+1, tmp_mat);
	//line 3
	imCum.ExtractMatrix(0, hei, wid-1, 1, tmp_mat);
	tmp_mat.RepeatGenerateMatrix(tmp_mat2, 1, r);
	imCum.ExtractMatrix(0, hei, wid-2*r-1, r, tmp_mat);
	tmp_mat = tmp_mat2 - tmp_mat;
	imDst.DistrictAssignMatrix(0, wid-r, tmp_mat);

	return 1;
}

template <class value_t>
int guidedfilter(general_matrix<value_t>& I, general_matrix<value_t>& p, int r, value_t eps, general_matrix<value_t>& q)
{
	int hei = I.GetMatrixHeight();
	int wid = I.GetMatrixWidth();
	general_matrix<value_t> N;
	general_matrix<value_t> tmp_mat(hei, wid, 1);
	boxfilter(tmp_mat, N, r);

	general_matrix<value_t> mean_I, mean_p, mean_Ip;
	boxfilter(I, tmp_mat, r);	
	mean_I.MatrixOptFilter( tmp_mat, N, '/' );
	boxfilter(p, tmp_mat, r);
	mean_p.MatrixOptFilter( tmp_mat, N, '/' );
	general_matrix<value_t> tmp_mat2;
	tmp_mat2.MatrixOptFilter( I, p, '*' );
	boxfilter( tmp_mat2, tmp_mat, r );
	mean_Ip.MatrixOptFilter( tmp_mat, N, '/' );

	tmp_mat.MatrixOptFilter( mean_I, mean_p, '*' );
	general_matrix<value_t> cov_Ip;
	cov_Ip.MatrixOptFilter( mean_Ip, tmp_mat, '-' );

	general_matrix<value_t> mean_II;
	tmp_mat2.MatrixOptFilter( I, I, '*' );
	boxfilter(tmp_mat2, tmp_mat, r);
	mean_II.MatrixOptFilter( tmp_mat, N, '/' );

	general_matrix<value_t> var_I;
	tmp_mat.MatrixOptFilter( mean_I, mean_I, '/' );
	var_I.MatrixOptFilter( mean_II, tmp_mat, '-' );

	tmp_mat2.ResizeMatrix( hei, wid, eps );
	tmp_mat.MatrixOptFilter( var_I, tmp_mat2, '+');

	general_matrix<value_t> a, b;
	a.MatrixOptFilter( cov_Ip, tmp_mat, '/' );

	tmp_mat.MatrixOptFilter( a, mean_I, '*' );
	b.MatrixOptFilter( mean_p, tmp_mat, '-');

	general_matrix<value_t> mean_a, mean_b;
	boxfilter( a, tmp_mat, r );
	mean_a.MatrixOptFilter( tmp_mat, N, '/' );
	boxfilter( b, tmp_mat, r );
	mean_b.MatrixOptFilter( tmp_mat, N, '/' );

	tmp_mat.MatrixOptFilter( mean_a, I, '*' );
	q.MatrixOptFilter( tmp_mat, mean_b, '+');

	return 1;
}


 