#pragma once
#include <base/typedefs.h>

//*
//	Solves general quadratic programming problems. 
//	Not intended for large sparse problems.
//
//	Minimizes:
//		min 1/2 * x'.G.x + g0'.x
//
//	Under constraints:
//		l_x <=  x  <= u_x
//		l_A <= A.x <= u_A
//*/

void solve_quadprog_nag(
	const dmatrix&	G,			// [n][n]  n = number of variables.
	const dvector&	g0,			// [n]
	const dvector&	l_x,		// [n]
	const dvector&	u_x,		// [n] 
	const dmatrix&	A,			// [nclin][n]  nclin = number of general linear constraint  ( or with lines padding a[nclin][tda] with tda>=n )
	const dvector&	l_A,		// [nclin]
	const dvector&	u_a,		// [nclin]
	// output
	dvector			&x,
	double			&objf, 
	std::string		&msg
	);

