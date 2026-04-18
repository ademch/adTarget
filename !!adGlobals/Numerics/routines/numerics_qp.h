#pragma once

/// Solves (convex) Quadratic Programming problem by means of an active-set dual method.
/// 
/// Minimizes: min 1/2 * x.G.x + g0.x
/// Under constraints: 
/// CE^T x + ce0 = 0
/// CI^T x + ci0 >= 0
///
/// Returns std::numeric_limits::infinity() if the problem is infeasible.
double solve_quadprog(
	dmatrix& G, dvector& g0, 
	const dmatrix& CE, const dvector& ce0,  
	const dmatrix& CI, const dvector& ci0, 
	// output
	dvector& x);

/// PLEASE DISREGARD THIS FUNCTION (AT LEAST TWO FUNCTIONS PER HEADERS FOR XLL SCRIPT)
int solve_quadprog_disregard(int a, int b);
