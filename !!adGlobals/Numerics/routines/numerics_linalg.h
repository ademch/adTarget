#pragma once
#include "base/typedefs.h"

/// Solves for a vector u[1..n] the tridiagonal linear set given by equation M.u=r. 
/// lower_diag[1] and upper_diag[n] are discarded.
darray1 tridiag_solve(
	const darray1&	lower_diag, 
	const darray1&	diag,
	const darray1&	upper_diag,
	const darray1&	r);

/// Build the plain matrix from tridiagonal vectors.
dmatrix tridiag_plain(
	const darray1&	a, 
	const darray1&	b,
	const darray1&	c);

darray2& cholesky_decomp(
	darray2& a);


void linest_alpha_ts(
	const darray2&	x,			///< data series. OLDER DATA FIRST!!!!
	const double 	alpha,		///< alpha=0 for usual equiweighted regression. alpha in ]0,1[ to use exponential moving average for X and X^2 e.g. ma <- alpha.x + (1-alpha).ma  Actualy max(alpha,1/i) is used for clean boot-strapping)
	const svector&	headers,	///< data column headers
	// output
	darray2&		betas,
	darray2&		ma,
	darray2&		var,
	darray2&		covar,
	dvector&		resvol,
	dvector&		explvar,
	svector&		covar_headers
	);

void linest_alpha(
	const darray1&	y,			///< explaining data series
	const darray2&	x,			///< explaining data series
	const bool		bDummy1,	///< optional. discarded. regression always done with constant
	const bool		bDummy2,	///< optional. discarded. always display stats
	const double 	alpha,		///< alpha=0 for usual equiweighted regression. alpha in ]0,1[ to use exponential moving average for X and X^2 e.g. ma <- alpha.x + (1-alpha).ma  Actualy max(alpha,1/i) is used for clean boot-strapping)
	// output
	darray2&		out
	);