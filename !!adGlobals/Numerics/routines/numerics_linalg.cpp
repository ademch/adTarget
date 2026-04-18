#include "numerics_linalg.h"
#include "base/XptionC.h"
#include "recipes/nr.h"

/// Agnostic version.
/// Solves for a vector u[1..n] the tridiagonal linear set given by equation M.u=r. 
/// lower_diag[1] and upper_diag[n] are discarded. u must be allocated.
void tridag(
	unsigned int  n,
	const double* lower_diag, 
	const double* diag, 
	const double* upper_diag, 
	const double* r, 
	double*		  u);

//////////////////////////////////////////////////////////////////////////


darray1 tridiag_solve(
	const darray1&	a, 
	const darray1&	b,
	const darray1&	c,
	const darray1&	r)
{
	require(a.size()!=0, "tridiag_solve: empty input");
	require(a.size()==r.size() && a.size()==b.size() && b.size()==c.size(), "tridiag_solve: input size mismatch");
	darray1 u(a.size());
	NR::tridag(a, b, c, r, u);
	return u;
}

void tridiag_solve(
	const dvector&	a, 
	const dvector&	b,
	const dvector&	c,
	const dvector&	r, 
	dvector&		u)
{
	require(a.size()!=0, "tridiag_solve: empty input");
	require(a.size()==r.size() && a.size()==b.size() && b.size()==c.size(), "tridiag_solve: input size mismatch");
	u.resize(a.size());
	tridag(a.size(), &a[0], &b[0], &c[0], &r[0], &u[0]);
}

dmatrix tridiag_plain(
	const darray1&	a, 
	const darray1&	b,
	const darray1&	c)
{
	uint n = b.size();
	dmatrix res(0.0, n, n);

	for (uint i=0; i<n; i++)
		res[i][i] = b[i];

	for (uint i=0; i<n-1; i++) {
		// a is lower diag [2..n]
		res[i+1][i] = a[i+1];
		// c is upper diag [1..n-1]
		res[i][i+1] = c[i];
	}

	return res;
}

void alpha_ma_ts(
	const dmatrix&	in,		// [nbPts][nbSeries]
	const double&	alpha,
	dmatrix&		out		// [nbPts][nbSeries]
	)
{
	uint n = rows(in), m = cols(in);
	resize_mtx(out, n, m);

	for (uint j=0; j<m; j++) 
		out[0][j] = in[0][j];

	for (uint i=1; i<n; i++) {
		const double a = max( alpha, 1.0/(i+1.0) ); // progressive alpha
		for (uint j=0; j<m; j++) 
			out[i][j] = in[i][j]*a + out[i-1][j]*(1.-a);
	}
}

void alpha_ma_last(
	const dmatrix&	in,			// [nbPts][nbSeries]
	const double&	alpha,		
	dvector&		out			// [nbSeries]
	)
{
	uint n = rows(in), m = cols(in);
	resize_vec(out, m);
	
	for (uint j=0; j<m; j++) 
		out[j] = in[0][j];
	
	for (uint i=1; i<n; i++) {
		const double a = max( alpha, 1.0/(i+1.0) ); // progressive alpha
		for (uint j=0; j<m; j++) 
			out[j]  = in[i][j]*a + out[j]*(1.-a);
	}
}

void alpha_ma2_ts(
	const dmatrix&	in,		// [nbPts][nbSeries]
	const double&	alpha,
	dmatrix&		ma2,	// [nbPts][nbSeries] E[X^2], matrix diagonal only
	dmatrix&		mab		// [nbPts][nbSeries*(nbSeries-1)/2] E[XY], strictly lower triangular matrix only, by rows 
	)
{
	uint n = rows(in), m = cols(in);
	resize_mtx(ma2, n, m);
	resize_mtx(mab, n, m*(m-1)/2);

	for (uint j=0; j<m; j++)
		ma2[0][j] = in[0][j]*in[0][j];
	for (uint j1=0, k=0; j1<m; j1++)
		for (uint j2=0; j2<j1; j2++, k++)
			mab[0][k] = in[0][j1]*in[0][j2];
	
	for (uint i=1; i<n; i++) {
		const double a = max( alpha, 1.0/(i+1.0) ); // progressive alpha
		for (uint j=0; j<m; j++)
			ma2[i][j] = in[i][j]*in[i][j]*a + ma2[i-1][j]*(1.-a);
		for (uint j1=0, k=0; j1<m; j1++)
			for (uint j2=0; j2<j1; j2++, k++)
				mab[i][k] = in[i][j1]*in[i][j2]*a + mab[i-1][k]*(1.-a);
	}
}

void alpha_ma2_last(
	const dmatrix&	in,		// [nbPts][nbSeries]
	const double&	alpha,
	dmatrix&		out 	// [nbSeries][nbSeries]
	)
{
	uint n = rows(in), m = cols(in);
	resize_mtx(out, m, m);
	
	for (uint j1=0; j1<m; j1++) 
		for (uint j2=0; j2<=j1; j2++) 
			out[j1][j2] = in[0][j1]*in[0][j2];
	
	for (uint i=1; i<n; i++) {
		const double a = max( alpha, 1.0/(i+1.0) ); // progressive alpha
		for (uint j1=0; j1<m; j1++) 
			for (uint j2=0; j2<=j1; j2++) 
				out[j1][j2] = in[i][j1]*in[i][j2]*a + out[j1][j2]*(1.-a);
	}
	
	// copy to upper triangle part of matrix
	for (uint j1=0; j1<m; j1++)
		for (uint j2=0; j2<j1; j2++)
			out[j2][j1] = out[j1][j2];
}

void headers_of_covar_ts(const svector& headers, svector& covar_headers) {
	uint m = headers.size();
	m = m*(m-1)/2;
	
	for (uint j1=0, k=0; j1<m; j1++)
		for (uint j2=0; j2<j1; j2++, k++) 
			covar_headers[k] = headers[j1] + " - " + headers[j2];
} 

// var and cov can be same vectors than ma2 and mab
void moments_to_covar_ts(
	const dmatrix&	ma,		// [nbPts][nbSeries]  E[x]
	const dmatrix&	ma2,	// [nbPts][nbSeries]  E[x^2] lower triangular matrix only, by rows 
	const dmatrix&	mab,	// [nbPts][nbSeries*(nbSeries-1)/2]   E[xy] lower triangular matrix only, by rows 
	// output
	dmatrix			&var,	// [nbPts][nbSeries]  Var[x]=E[(x-mu)^2] lower triangular matrix only, by rows 
	dmatrix			&cov    // [nbPts][nbSeries*(nbSeries-1)/2]  Cov[x,y] = E[(x-mu)(y-no)] lower triangular matrix only, by rows 
	)
{
	uint n = rows(ma), m = cols(ma);

	for (uint i=1; i<n; i++) {
		for (uint j=0; j<m; j++)
			var[i][j] = ma2[i][j] - ma[i][j]*ma[i][j];
		for (uint j1=0, k=0; j1<m; j1++)
			for (uint j2=0; j2<j1; j2++, k++) 
				cov[i][k] = mab[i][k] - ma[i][j1]*ma[i][j2];
	}
}

void moments_to_covar_last(
	const dvector&	ma,		// [nbSeries]
	const dmatrix&	ma2,	// [nbSeries][nbSeries]
	// output
	dmatrix			&cov    // [nbSeries][nbSeries]
	)
{
	uint m = ma.size();

	for (uint j1=0; j1<m; j1++)
		for (uint j2=0; j2<m; j2++) 
			cov[j1][j2] = ma2[j1][j2] - ma[j1]*ma[j2];
}

void alpha_betas_resvol_last(
	const dmatrix&	covar,		// [nbSeries][nbSeries]  Covariance matrix
	double			*betas,		// [nbSeries]  axis of PCA first factor
	double			&resvol,	// vol of residual along remaining axes
	double			&explvar	// explained variance
	)
{
	uint n = rows(covar), m = cols(covar);

	// total variance
	double total_variance = 0.0;
	for (uint j=0; j<m; j++)
		total_variance += covar[j][j]; 

	// find the first PCA component vector
	darray1 w; dmatrix v;
	NR::svdcmp(dmatrix(covar), w, v);		// COPY !!!
	// find the greatest variance
	uint first_comp_index = 0;
	double first_comp_variance = 0.0;
	for (uint j=0; j<m; j++)
		if (w[j]>first_comp_variance) {
			first_comp_index = j;
			first_comp_variance = w[j]; }
	// keep results
	for (uint j=0; j<m; j++)
		betas[j] = v[j][first_comp_index];
	resvol = sqrt(total_variance-first_comp_variance);
	explvar = first_comp_variance / total_variance;
}

void alpha_betas_resvol_ts(
	const dmatrix&	var,	// [nbPts][nbSeries]  E[x^2] lower triangular matrix only, by rows 
	const dmatrix&	cov,	// [nbPts][nbSeries*(nbSeries-1)/2]   E[xy] lower triangular matrix only, by rows 
	dmatrix &		betas,	// [nbPts][nbSeries]  axis of PCA first factor
	dvector &		resvol,	// [nbPts]  vol of residual along remaining axes
	dvector &		explvar	// [nbPts]  explained variance
	)
{
	uint n = rows(var), m = cols(var);
	resize_mtx(betas, n, m);
	resize_vec(resvol, n);
	resize_vec(explvar, n);

	// temporary covar matrix
	dmatrix covar(0.0, m, m);

	for (uint i=1; i<n; i++) {
		
		// build covariance matrix
		for (uint j=0; j<m; j++)
			covar[j][j] = var[i][j];
		for (uint j1=0, k=0; j1<m; j1++)
			for (uint j2=0; j2<j1; j2++, k++) 
				covar[j1][j2] = covar[j2][j1] = cov[i][k];

		alpha_betas_resvol_last( covar,	
			betas[i], resvol[i], explvar[i]);
	}
}

double sq(double x) { return x*x; }

void linest_alpha_ts(
	const darray2&	x,
	const double 	alpha,
	const svector&	headers,	///< data column headers
	// output
	darray2&		betas,
	darray2&		ma,
	darray2&		var,
	darray2&		covar,
	dvector&		resvol,
	dvector&		explvar,
	svector&		covar_headers
	)
{
	uint n = rows(x);
	uint m = cols(x);
	require(alpha>=0 && alpha<=1, "alpha %f must be in [0,1], alpha=0 meaning equi-weighted", alpha);

	alpha_ma_ts(x, alpha, ma);	// first moments (means)
	alpha_ma2_ts(x, alpha, var, covar);	// second moments (for variances/covariances)
	moments_to_covar_ts(ma, var, covar, var, covar);   // moments into covariances
	headers_of_covar_ts(headers, covar_headers); 
	alpha_betas_resvol_ts(var, covar, betas, resvol, explvar);
}

/// Time series exponentially weighted linear regression.
void linest_alpha(
	const darray1&	y,
	const darray2&	x,				
	const bool		bDummy1,	// optional. discarded. regression always done with constant
	const bool		bDummy2,	// optional. discarded. always display stats
	const double 	alpha,			
	//const int		nbFactors,		// factors<0 ordinary least square, factors>0 orthogonal least square (1st vector of PCA), we out put the first nbFactors of the PCA. factors=0, just moving avergaes.
	// output
	darray2&		out)
{
	uint n = rows(x);
	uint m = cols(x);
	require(alpha>=0 && alpha<=1, "alpha %f must be in [0,1], alpha=0 meaning equi-weighted", alpha);
	require(size(y)==n, "y and x don't have same number of rows. %i vs %i", size(y), n);

	double ma_x  = x[0][0];
	double ma_xx = x[0][0]*x[0][0];
	double ma_y  = y[0];
	double ma_yy = y[0]*y[0];
	double ma_xy = x[0][0]*y[0];
	if (alpha>0) {
		// exponentially weighted average 
		for (uint i=1; i<n; i++) {
			ma_x  = x[i][0]*alpha + ma_x*(1.-alpha);
			ma_xx = x[i][0]*x[i][0]*alpha + ma_xx*(1.-alpha);
			ma_y  = y[i]*alpha + ma_y*(1.-alpha);
			ma_yy = y[i]*y[i]*alpha + ma_yy*(1.-alpha);
			ma_xy = x[i][0]*y[i]*alpha + ma_xy*(1.-alpha);
		}
	} else {
		// equi-weighted average 
		for (uint i=1; i<n; i++) {
			ma_x  += x[i][0];
			ma_xx += x[i][0]*x[i][0];
			ma_y  += y[i];
			ma_yy += y[i]*y[i];
			ma_xy += x[i][0]*y[i];
		}
		ma_x /= n;
		ma_xx /= n;
		ma_y /= n;
		ma_yy /= n;
		ma_xy /= n;
	}

	double det = ma_xx - ma_x*ma_x;
	double beta = ( ma_xy - ma_x * ma_y ) / det;
	double cste = ( - ma_xy * ma_x + ma_xx * ma_y ) / det;

	double det_inv = ma_yy - ma_y*ma_y;
	double beta_inv = ( ma_xy - ma_x * ma_y ) / det_inv;
	double cste_inv = ( - ma_xy * ma_y + ma_yy * ma_x ) / det_inv;

	double yy = ma_yy - ma_y*ma_y;
	double xx = ma_xx - ma_x*ma_x;
	double xy = ma_xy - ma_x*ma_y;
	double beta_pca = ( (yy-xx) + sqrt(sq(yy+xx)-4*(yy*xx-sq(xy))) ) / (2*xy);
	
	double r2 = beta*beta_inv, estimate_stdev = 0.0;

	resize_mtx(out, 3, m+1); 
	// first row
	for (uint j=0; j<m; j++) out[0][j] = beta; //[j];
	out[0][m] = cste;
	// second row (differs from linest)
	for (uint j=0; j<m; j++) out[1][j] = beta_inv; //[j];
	out[1][m] = cste_inv;
	// third row 
	out[2][0] = r2;
	out[2][1] = beta_pca; // standard deviation of y estimate
	// fourth and fifth row 
}
