#pragma once
#include <vector>
typedef std::vector<double>	dvector;

// Greatest Common Divider for floating number
double youssef_gcd(
	const dvector&	x_dbl,			///< positive doubles
	long			max_nb_pts		///< constraint : sum(x_i)/gcd < max_nb_pts
	);
