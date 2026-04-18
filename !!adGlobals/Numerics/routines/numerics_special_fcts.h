#pragma once

/// incomplete Beta function I(a,b,x) = 1/B(a,b) . Int_0^x t^(a?1).(1 ? t)^(b?1) dt
double NR::betai(
	double a,			/// a > 0
	double b,			/// b>0
	double x			/// must be in ]0,1[.
	);

/// cumulative normal distribution
double normal_cumulative(
	double x			/// [-infty,+infty]
	);

/// inverse normal cumulative distribution
double normal_cumulative_inv(
	double u			/// [0,1]
	);

// normal density : f(x) = 1/sqrt(2pi) * exp(-x^2/2)
double normal_distribution(double x);
