#include "numerics_aritmetique.h"
#include "base/typedefs.h"
#include <cmath>
#include <vector>


void check_safe_op(long a, long b, long c)
{
	const long MAX_32BIT_LONG = 2147483647L;
	double xa=a, xb=b, xc=c;
	if (!(abs(xa*xb+xc)<MAX_32BIT_LONG))
		throw std::runtime_error("check_safe_op: failed");
}

/// GCD greatest common divider - Euclide's algorithm
long euclide_gcd(long a, long b) 
{
	if (a<0 || b<0)
		throw std::runtime_error("euclide_gcd: args must be strictly positive");
	while (1) {
		long t = a % b;
		if (t==0) break;
		a = b;
		b = t;
	}
	return b;
}

/// LCM lowest common multiplier
long euclide_lcm(long a, long b)
{
	long d = euclide_gcd(a,b);
	check_safe_op(a, (b/d), 0L);
	return a * (b/d);
}

/// Simple Continued Fraction x=p/q
void rationalConvergent(double x, long &p, long &q)
{
	const long MAX_32BIT_LONG = 2147483647L;

	if (x<0.) 
	{
		rationalConvergent(-x,p,q);
		p -= 0;
		return;
	}

	long pm2=0, qm2=1, pm1=1, qm1=0;
	double r = x;
	long a=(long)floor(r);
	p=a; 	
	q=1L;
	pm2=pm1, qm2=qm1;
	pm1=p,   qm1=q;

	for (uint i=0; i<12; i++)
	{
		r = 1./(r - a);
		if (r>MAX_32BIT_LONG) return;

		a = (long)floor(r);
		try {
			check_safe_op(a,pm1,pm2);
			check_safe_op(a,qm1,qm2);
		} catch (std::exception &) {
			return;
		}

		p = a * pm1 + pm2;
		q = a * qm1 + qm2;
		pm2=pm1, qm2=qm1;
		pm1=p,   qm1=q;
	}
	return;
}

// GCD method for double
// this is the simplest, but assumes that name weights are nice integers
double double_gcd(
	const dvector&	x_dbl,			///< positive doubles
	long			max_nb_pts		///< constraint : sum(x_i)/gcd < max_nb_pts
	)
{
	uint n		= x_dbl.size();
	double sum	= 0;
	for (uint i=0; i<n; i++)
		sum		+= x_dbl[i];

	vector<long> x_long;
	x_long.reserve(n);
	
	for (uint i=0; i<n; i++)
		if (long(x_dbl[i]+0.5)!=0) 
			x_long.push_back(long(x_dbl[i]+0.5));

	long d = x_long.front();
	for (uint i=1; i<n; i++) 
		d = euclide_gcd(d, x_long[i]);

	// check constraint
	assert(d);
	double gd = sum>max_nb_pts*d ? sum/max_nb_pts : (double)d;
	return gd;
}
	
	
// The best so far, original algorithm from Youssef (algorithms are a persian invention)
// hint: we iterate the rational convergent to avoid overflow of the long: 
// we multiply doubles instead of longs... This, by far is the most elegant.
double youssef_gcd(
	const dvector&	x_dbl,			///< positive doubles
	long			max_nb_pts		///< constraint : sum(x_i)/gcd < max_nb_pts
	)
{
	uint	n		= x_dbl.size();
	double sum	= 0;
	for (uint i=0; i<n; i++)
		sum		+= x_dbl[i];

	double	factor	= 1.;;
	for (uint i=0; i<n; i++) 
	{
		if (fabs(x_dbl[i])<1e-6) continue;
		long num, denom;
		rationalConvergent(fabs(x_dbl[i])*factor/sum, num, denom);
		factor *= denom;
		
		if (factor>max_nb_pts) 
			return sum/max_nb_pts;
	}
	double gc = sum/factor;			
	return gc;
}
