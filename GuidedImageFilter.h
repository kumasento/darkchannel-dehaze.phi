/*
 * This is based on the original MATLAB version from the author
 * Created on 06/22/2014
 */

//#define PRESENT_MATLAB_LINE_NUM
#define PRINT PrintMatrixArray

#define PLUS(x, y) MatrixOptFilter( (x), (y), '+' )
#define MINUS(x, y) MatrixOptFilter( (x), (y), '-' )
#define MUL(x, y) MatrixOptFilter( (x), (y), '*' )
#define DIV(x, y) MatrixOptFilter( (x), (y), '/' )

#define CAL_VAR(dst, src_11, src_12, src_21, src_22, tmp_1, tmp_2, r) \
(tmp_1).MUL( (src_11), (src_12) );\
boxfilter((tmp_1), (tmp_2), (r));\
(tmp_1).MUL( (src_21), (src_22) );\
(dst).MINUS( (tmp_2), (tmp_1) );

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
	int hei = p.GetMatrixHeight();
	int wid = p.GetMatrixWidth();

	printf("hei: %d wid: %d\n", hei, wid);
	general_matrix<value_t> N;
	general_matrix<value_t> tmp_mat(hei, wid, 1);
	boxfilter(tmp_mat, N, r);

	general_matrix<value_t> mean_I, mean_p, mean_Ip;
	
	boxfilter(I, tmp_mat, r); mean_I.DIV( tmp_mat, N );
	boxfilter(p, tmp_mat, r); mean_p.DIV( tmp_mat, N );
	general_matrix<value_t> tmp_mat2;
	tmp_mat2.MUL( I, p ); 
	boxfilter( tmp_mat2, tmp_mat, r );
	tmp_mat.PRINT();
	N.PRINT();
	mean_Ip.DIV( tmp_mat, N );

	tmp_mat.MUL( mean_I, mean_p );
	general_matrix<value_t> cov_Ip; cov_Ip.MINUS( mean_Ip, tmp_mat );

	general_matrix<value_t> mean_II;
	tmp_mat2.MUL( I, I );
	boxfilter(tmp_mat2, tmp_mat, r); mean_II.DIV( tmp_mat, N );

	general_matrix<value_t> var_I;
	tmp_mat.MUL( mean_I, mean_I ); var_I.MINUS( mean_II, tmp_mat );

	tmp_mat2.ResizeMatrix( hei, wid, eps );
	tmp_mat.PLUS( var_I, tmp_mat2 );

	general_matrix<value_t> a, b;

	a.DIV( cov_Ip, tmp_mat );
	tmp_mat.MUL( a, mean_I );
	b.MINUS( mean_p, tmp_mat );

	general_matrix<value_t> mean_a, mean_b;
	boxfilter( a, tmp_mat, r ); mean_a.DIV( tmp_mat, N );
	boxfilter( b, tmp_mat, r ); mean_b.DIV( tmp_mat, N );

	tmp_mat.MUL( mean_a, I );
	q.PLUS( tmp_mat, mean_b );

	return 1;
}

template <class value_t>
int guidedfilter_color(general_matrix<value_t>& I_r,
					   general_matrix<value_t>& I_g,
					   general_matrix<value_t>& I_b,	 
					   general_matrix<value_t>& p, 
					   int r, 
					   value_t eps, 
					   general_matrix<value_t>& q)
{
	int hei = p.GetMatrixHeight();
	int wid = p.GetMatrixWidth();
	general_matrix<value_t> N;
	general_matrix<value_t> tmp_mat(hei, wid, 1);
	boxfilter(tmp_mat, N, r);

	general_matrix<value_t> mean_I_r, mean_I_g, mean_I_b;
	general_matrix<value_t> mean_p;
	general_matrix<value_t> mean_Ip_r, mean_Ip_g, mean_Ip_b;
	boxfilter(I_r, tmp_mat, r); mean_I_r.DIV( tmp_mat, N ); // mean_I_r = boxfilter(I(:, :, 1), r) ./ N;
	boxfilter(I_g, tmp_mat, r); mean_I_g.DIV( tmp_mat, N ); // mean_I_g = boxfilter(I(:, :, 2), r) ./ N;
	boxfilter(I_b, tmp_mat, r); mean_I_b.DIV( tmp_mat, N ); // mean_I_b = boxfilter(I(:, :, 3), r) ./ N;

	boxfilter(p, tmp_mat, r); mean_p.DIV( tmp_mat, N ); // mean_p = boxfilter(p, r) ./ N;

	general_matrix<value_t> tmp_mat2;
	// mean_Ip_r = boxfilter(I(:, :, 1).*p, r) ./ N;
	tmp_mat2.MUL( I_r , p ); boxfilter( tmp_mat2, tmp_mat, r ); mean_Ip_r.DIV( tmp_mat, N );
	// mean_Ip_g = boxfilter(I(:, :, 1).*p, r) ./ N;
	tmp_mat2.MUL( I_g , p ); boxfilter( tmp_mat2, tmp_mat, r ); mean_Ip_g.DIV( tmp_mat, N );
	// mean_Ip_b = boxfilter(I(:, :, 1).*p, r) ./ N;
	tmp_mat2.MUL( I_b , p ); boxfilter( tmp_mat2, tmp_mat, r ); mean_Ip_b.DIV( tmp_mat, N );

	//cov_Ip_r = mean_Ip_r - mean_I_r .* mean_p;
	general_matrix<value_t> cov_Ip_r, cov_Ip_g, cov_Ip_b;
	tmp_mat.MUL( mean_I_r, mean_p ); cov_Ip_r.MINUS( mean_Ip_r, tmp_mat );
	tmp_mat.MUL( mean_I_g, mean_p ); cov_Ip_g.MINUS( mean_Ip_g, tmp_mat );
	tmp_mat.MUL( mean_I_b, mean_p ); cov_Ip_b.MINUS( mean_Ip_b, tmp_mat );

	general_matrix<value_t> var_I_rr, var_I_rg, var_I_rb, var_I_gg, var_I_gb, var_I_bb;
	CAL_VAR(var_I_rr, I_r, I_r, mean_I_r, mean_I_r, tmp_mat, tmp_mat2, r);
	CAL_VAR(var_I_rg, I_r, I_g, mean_I_r, mean_I_g, tmp_mat, tmp_mat2, r);
	CAL_VAR(var_I_rb, I_r, I_b, mean_I_r, mean_I_b, tmp_mat, tmp_mat2, r);
	CAL_VAR(var_I_gg, I_g, I_g, mean_I_g, mean_I_g, tmp_mat, tmp_mat2, r);
	CAL_VAR(var_I_gb, I_g, I_b, mean_I_g, mean_I_b, tmp_mat, tmp_mat2, r);
	CAL_VAR(var_I_bb, I_b, I_b, mean_I_b, mean_I_b, tmp_mat, tmp_mat2, r);
	
	return 1;
}

 
