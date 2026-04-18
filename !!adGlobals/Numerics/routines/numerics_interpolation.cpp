#include "numerics_interpolation.h"
#include <algorithm>
#include <cmath>
#include "vision/matrix-basics.hpp"
#include "base/typedefs.h"
#include "base/XptionC.h"
Mat_DP luinv(Mat_I_DP& mat);

namespace {
	double dmin(double a, double b) { return a<b ? a : b; }
	double dmax(double a, double b) { return a<b ? b : a; }
	double sq(double a) { return a*a; }
	double cube(double a) { return a*a*a; }
}


// linear interp, flat extrap
double interp_linear(
	const uint		n,
	const double*	x_in,
	const double*	y_in,
	const double	x
	)
{
	const double epsilon = 1e-11;
	
	uint i = std::lower_bound(x_in, x_in+n, x) - x_in;

	if (i==0)
		//if (x+epsilon<x_in[0])
			//throw std::runtime_error("interp_linear: before first point");
		return y_in[0];	
	if (i==n)
		//if (x_in[i-1]<x-epsilon)
			//throw std::runtime_error("interp_linear: after last point");
		return y_in[i-1]; 

	double xl = x_in[i-1], xr = x_in[i];
	double yl = y_in[i-1], yr = y_in[i];
	double delta = (yr-yl)/(xr-xl);

	return yl + delta*(x-xl);
}


double interp_linear(
	const dvector&	x_in,
	const dvector&	y_in,
	const double	x
	)
{
	const uint n = x_in.size();
	return interp_linear(n, &x_in[0], &y_in[0], x);
}

// Costantini - Morandi C1 interpolation, preserving monotonicity and convexity (1983)
// piece-wise degree 3 Hermite polynomials

dvector como(
	const dvector&	x_in,
	const dvector&	y_in,
	const dvector&	d_in,
	const dvector&	x_out
	)
{
	const double epsilon = 1e-11;
	dvector y_out(x_out.size());

	for (uint k=0; k<x_out.size(); k++)
	{
		double x = x_out[k];
		uint i = std::lower_bound(x_in.begin(), x_in.end(), x) - x_in.begin();
		
		if (i==0) {
			y_out[k] = y_in[0];
			if (x+epsilon<x_in[0])
				throw std::runtime_error("como_a0");
			continue;
		}
		if (i==x_in.size()) {
			y_out[k] = y_in[i-1];
			if (x_in[i-1]<x-epsilon)
				throw std::runtime_error("como_an");
			continue;
		}
		
		double xl = x_in[i-1], xr = x_in[i];
		double yl = y_in[i-1], yr = y_in[i];
		double dl = d_in[i-1], dr = d_in[i];
		double delta = (y_in[i]-y_in[i-1])/(x_in[i]-x_in[i-1]);

		y_out[k] = (dl+dr-2.0*delta)/sq(xr-xl) * cube(x-xl)
				+ (-2.0*dl-dr+3.0*delta)/(xr-xl) * sq(x-xl)
				+ (dl) * (x-xl) + yl;
	}

	return y_out;
}

// written for increasing concave
// other shape (decreasing-concave, decreasing-convex, increasing convex) obtained by symmetry.
bool como_a1(
	const dvector&	delta,
	dvector&		d_min,
	dvector&		d_max
	)
{
	const int nb_pts = delta.size()+1;	// x, y [0..n-1], delta [0..n-2]
	const int N = nb_pts-1;
	d_min.resize(nb_pts);
	d_max.resize(nb_pts);
	
	for (int i=N; i>-1; i--) {
		double d_min_rgt = i!=N ? d_min[i+1] : -1e99;
		double d_max_rgt = i!=N ? d_max[i+1] : 0.0;
		double delta_lft = i!=0 ? delta[i-1] : +1e99; 
		double delta_rgt = i!=N ? delta[i]   : 0.0; 

		d_min[i] = - d_max_rgt/2.0 + 3.0*delta_rgt/2.0	;
		d_max[i] = dmin(delta_lft, -2.0*d_min_rgt + 3.0*delta_rgt);

		if (d_min[i]>d_max[i])
			throw std::runtime_error("como_a1");
	}
	return true;
}

void como_a2(
	const dvector&	delta,
	const dvector&	d_min,
	const dvector&	d_max,
	const double	lambda,
	dvector&		d
	)
{
	const uint n = d_min.size();
	d.resize(n);

	d[0] = lambda*d_min[0] + (1.-lambda)*d_max[0];

	for (uint i=1; i<n; i++)
	{
		double dn = dmax(d_min[i], dmax( 3.0*delta[i-1]-2.0*d[i-1] , 0.0 ));
		double up = dmin(d_max[i], dmin((3.0*delta[i-1]-d[i-1])/2.0 , 3*delta[i-1]));

		if (i+1<n) {
			// a personal variation
			double d0 = (delta[i-1]+delta[i])/2.0;
			if (dn<d0 && d0<up) {
				d[i] = d0;
				continue;
			}
		} 
		
		// recommended in paper
		d[i] = lambda*dn + (1.-lambda)*up;
	}
}

dvector como_increasing_concave(
	const dvector&	x,
	const dvector&	y,
	const double	lambda
	)
{
	// adjacent_difference:		involutive function eg. f o f = id
	// - the element at start will be copied to result
	// - then the difference between the element at start + i and start + (i-1) will be stored at result + i

	const uint n = x.size();
	
	dvector delta(n-1);
	for (uint i=1; i<n; i++)
		delta[i-1] = (y[i]-y[i-1])/(x[i]-x[i-1]);

	dvector d_min, d_max, d;
	como_a1(delta, d_min, d_max);
	como_a2(delta, d_min, d_max, lambda, d);
	
	return d;
}	

//////////////////////////////////////////////////////////////////////////

// coerce slopes to be positive in increasing==true, or negative if increasing==false
void coerce_slope_monotonic(
	dvector&		slopes,
	bool			increasing)
{
	uint n = slopes.size();
	if (increasing)
		for (uint i=0; i<n; i++)
			slopes[i] = dmax(slopes[i], 0.0);
	else
		for (uint i=0; i<n; i++)
			slopes[i] = dmin(slopes[i], 0.0);
}

// calculate mid-slopes
dvector calculate_mid_slopes(
	const dvector&	x_in,
	const dvector&	y_in,
	const bool		monotonic,
	const double	slope_begin,
	const double	slope_end,
	const double	slope_wght
	)
{
	int i=0, n = x_in.size();
	dvector d(n);

	if (n<2)
		throw std::runtime_error("calculate_mid_slopes: x_in, y_in must be at least of size 2");

	d[0] = (y_in[i+1]-y_in[i])/(x_in[i+1]-x_in[i])*(1.-slope_wght) + slope_begin*slope_wght;
	for (i=1; i<n-1; i++)
		d[i] = ( (y_in[i+1]-y_in[i])/(x_in[i+1]-x_in[i]) + (y_in[i]-y_in[i-1])/(x_in[i]-x_in[i-1]) ) / 2; 
	d[i] = (y_in[i]-y_in[i-1])/(x_in[i]-x_in[i-1])*(1.-slope_wght) + slope_end*slope_wght;

	if (monotonic)
		coerce_slope_monotonic(d, y_in[n-1]>y_in[0]);

	return d;
}


double interp_ellipse_increasing_concave(
	double	x,		///< wanted abscissa xl < x < xr 
	double	xl,		///< ellipse should go through point (xl,yl) with tangent having (dl) slope 
	double	xr,		///< idem
	double	yl,		
	double	yr, 
	double	dl, 
	double	dr, 
	double	lambda,	///< [0..1] additional constraint : 0 tends toward linear interpolation, 1 is maximum concavity
	bool	lower_branche
	)
{
	// point P is the intersection of tangent
	double xp = ( (yr-dr*xr) - (yl-dl*xl) ) / (dl-dr);
	double yp = yl + dl*(xp-xl);

	// point LR is the middle of segment [LR]
	// point LR is also the middle of segment [QP]
	
	// point M is third point through which the ellipse goes through
	// M = (1-mu)*Q + mu * P		with 0.5 < mu < 1/sqrt(2), simplified into 0 < lambda < 1
	//   = (1-mu)*(L+R)+(2mu-1)*P
	double mu =  0.5*(1-lambda) + lambda/std::sqrt(2.0);
	double xm = (1.0-mu)*(xl+xr) + (2*mu-1)*xp;
	double ym = (1.0-mu)*(yl+yr) + (2*mu-1)*yp;
	
	// we switch to barycentric coordinates in (L,R,M)
	// coordinates are L(1,0,0), R(0,1,0), M(0,0,1)
	// coordinates of P is P = (mu-1)/(2mu-1)*(L+R) + M/(2mu-1) : P((1-k)/2,(1-k)/2,k)   with k=1/(2mu-1) 
	double k = 1/(2*mu-1);
	dmatrix H(3,3);
	H[0][0] = H[1][1] = H[2][2] = 0.0;
	H[0][1] = H[1][0] = 2*k/(k-1);
	H[0][2] = H[2][0] = H[1][2] = H[2][1] = 1.0;
	
	dmatrix P(3,3);
	P[0][0] = xl; P[0][1] = xr; P[0][2] = xm;
	P[1][0] = yl; P[1][1] = yr; P[1][2] = ym;
	P[2][0] = P[2][1] = P[2][2] = 1.0;

	dmatrix Pi = luinv(P);
	
	dmatrix Pit_H_Pi = trsp(Pi)*H*Pi;
	double a = Pit_H_Pi[0][0];					// x^2
	double c = Pit_H_Pi[1][1];					// y^2
	double b = Pit_H_Pi[0][1]+Pit_H_Pi[1][0];	// xy
	double d = Pit_H_Pi[0][2]+Pit_H_Pi[2][0];	// x
	double e = Pit_H_Pi[1][2]+Pit_H_Pi[2][1];	// y
	double f = Pit_H_Pi[2][2];					// 1
	double ok_neg = b*b - 4*a*c;

	double deg2_a = c;
	double deg2_b = b*x + e;
	double deg2_c = a*x*x  + d*x + f;
	double deg2_discr = deg2_b*deg2_b -4*deg2_a*deg2_c;
	
	// discr_mult selectes which branch of the conique we use for interpolation (higher or lower branch)
	double discr_mult = (deg2_a>0.?1.:-1.)*(lower_branche?-1.:1.);
	double y = (-deg2_b+discr_mult*sqrt(deg2_discr)) / (2*deg2_a);
	return y;
}

dvector interp_conique_no_inflexion(
	const dvector&	x_in,
	const dvector&	y_in,
	const dvector&	d_in,
	const double	lambda,
	const dvector&	x_out,
	const bool		linear_if_concave,
	const bool		linear_if_convex,
	const bool		lower_branche
	)
{
	const uint n = x_in.size();

	const double epsilon = 1e-11;
	dvector y_out(x_out.size());

	for (uint k=0; k<x_out.size(); k++)
	{
		double x = x_out[k];
		uint i = std::lower_bound(x_in.begin(), x_in.end(), x) - x_in.begin();

		if (i==0) {
			y_out[k] = y_in[0];
			if (x+epsilon<x_in[0])
				throw std::runtime_error("interp_conique_no_inflexion: before first point");
			continue;
		}
		if (i==x_in.size()) {
			y_out[k] = y_in[i-1];
			if (x_in[i-1]<x-epsilon)
				throw std::runtime_error("interp_conique_no_inflexion: after last point");
			continue;
		}

		double xl = x_in[i-1], xr = x_in[i];
		double yl = y_in[i-1], yr = y_in[i];
		double dl = d_in[i-1], dr = d_in[i];
		double delta = (yr-yl)/(xr-xl);

		const double eps = 1e-11;
		if (dl-delta>eps && delta-dr>eps && lambda>eps)
		{
			// concave case
			// conic interpolation is possible

			if (linear_if_concave)
				y_out[k] = yl + delta*(x-xl);
			else
				y_out[k] = interp_ellipse_increasing_concave(
					x_out[k], xl, xr, yl, yr, dl, dr, lambda, lower_branche);
		}
		else if (delta-dl>eps && dr-delta>eps && lambda>eps)
		{
			// convex case
			// conic interpolation is possible (with -f)

			if (linear_if_convex)
				y_out[k] = yl + delta*(x-xl);
			else
				y_out[k] = - interp_ellipse_increasing_concave(
					x_out[k], xl, xr, -yl, -yr, -dl, -dr, lambda, lower_branche);
		}
		else
		{
			// limit case (or bad input) : degenerate to linear interpolation
			y_out[k] = yl + delta*(x-xl);
		}
	}
	return y_out;
}

dvector interp_conique_increasing_concave(
	const dvector&	x_in,
	const dvector&	y_in,
	const dvector&	d_in,
	const double	lambda,
	const dvector&	x_out)
{
	return interp_conique_no_inflexion(
		x_in, y_in, d_in, lambda, x_out,
		false, true, false);
}


dvector increasing_concave_envelop(
	const dvector&	x_in,
	const dvector&	y_in)
{
	const char* fct = "increasing_concave_envelop";
	require(y_in.front() <= y_in.back(),	"%s: non increasing input", fct);
	require(x_in.size() == y_in.size(),		"%s: input size mismatch", fct);

	const uint	n = x_in.size();
	dvector		x = x_in;	// ref
	dvector		y = y_in;	// copy

	// ensure we are bound my first and last values
	const double low	= y[0];
	const double high	= y[n-1];
	const double epsilon = 1e-11;
	for (uint i=1; i<n-1; i++) {
		require(y[i]-epsilon<high,	"%s: %f not capped by last value %f.", fct, y[i], high);
		require(low<y[i]+epsilon,	"%s: %f not floored by first value %f.", fct, y[i], low);
		y[i] = min(max(low, y[i]), high);
	}

	// ensure we are concave
	bool is_concave = false;
	while (!is_concave) 
	{
		is_concave = true;

		for (uint i=1; i<x.size()-1; i++) 
		{
			double y_linear = ( (x[i+1]-x[i])*y[i-1] + (x[i]-x[i-1])*y[i+1] ) / (x[i+1]-x[i-1]);
			if (y[i]+epsilon<y_linear) 
			{
				is_concave = false;
				x.erase(x.begin()+i);
				y.erase(y.begin()+i);
				break;
			}
		}
	}

	// replace removed value by their linear interpolation
	dvector		y_out(y_in);	// copy
	for (uint i=1; i<n-1; i++) 
		if (find(x.begin(),x.end(),x_in[i])==x.end())
			y_out[i] = interp_linear(x, y, x_in[i]);	
	return y_out;
}

