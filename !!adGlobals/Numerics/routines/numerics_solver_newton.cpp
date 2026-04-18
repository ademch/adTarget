#include "numerics_solver_newton.h"
#include "base/XptionC.h"

solver_newton::solver_newton(
	double	x_min, 
	double	x_max, 
	uint	max_iter
) : x1(x_min), x2(x_max), nb_iter(0), max_iter(max_iter), error(false) {}

bool solver_newton::err()
{
	return error!=0;
}

double solver_newton::next_x(double& x, double  y)
{
	// if called while an error message has been set we throw
	require(!error, error);

	if (nb_iter==0) {
		x1 = x;
		y1 = y;
		nb_iter++;
		return x2;
	} else if (nb_iter==2) {
		x2 = x;
		y2 = y;
	} else {
		if (y1*y<0.0) {
			x2 = x;
			y2 = y;
		} else {
			x1 = x;
			y1 = y;
		}
	}
	
	//require(y1*y2<0.0, "Newton next_xn (not bracketing): x1=%f, y1=%f, x2=%f, y2=%f", x1, y1, x2, y2);
	// error = ""Newton next_xn (not bracketing)";

	nb_iter++;
	if (nb_iter>max_iter) 
		error = "Newton next_xn (more than max_iter)";

	return x = (x1*y2-x2*y1)/(y2-y1);
}

