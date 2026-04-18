#pragma once

typedef unsigned int	uint;

struct solver_newton
{
	solver_newton(
		double	x_min, 
		double	x_max, 
		uint	max_iter);

	double	next_x(double& x, double  y);
	bool	err();

	double  x, x1, x2, y1, y2;
	uint	nb_iter, max_iter;
	const char*	error;
};
