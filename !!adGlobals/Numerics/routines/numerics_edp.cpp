#include <base/typedefs.h>

void tridiag_solve(
	const dvector&	a, 
	const dvector&	b,
	const dvector&	c,
	const dvector&	r, 
	dvector&		u);

// f_t = a.f + b.f_x + c.f_xx

// (f[s+1,i]-f[s,i])/dt 	 
//	= theta  .    ( a.f[s+1,i] + b.(f[s+1,i+1]-f[s+1,i-1])/{2dx} + c.(f[s+1,i+1]-f[s+1,i]+f[s+1,i-1])/{dx^2} )
//  + (1-theta) . ( a.f[s,i]   + b.( f[s, i+1]- f[s, i-1])/{2dx} + c.( f[s, i+1]- f[s, i]+ f[s, i-1])/{dx^2} )

// f[s+1,i]/dt -   theta  .  ( a.f[s+1,i] + b.(f[s+1,i+1]-f[s+1,i-1])/{2dx} + c.(f[s+1,i+1]-f[s+1,i]+f[s+1,i-1])/{dx^2} )
// = f[s,i]/dt + (1-theta) . ( a.f[s,i]   + b.( f[s, i+1]- f[s, i-1])/{2dx} + c.( f[s, i+1]- f[s, i]+ f[s, i-1])/{dx^2} )

// f_1/dt - theta.L^1(f_1) =  f_0/dt + (1-theta).L^0(f_0)
// [L^1-1/(theta*dt)](f_1) =  - [ f_0/(theta*dt) + (1-theta)/theta.L^0(f_0) ]


void make_grid_1d_min_max_nb(
	double min, double max, int nb, dvector& grid)
{
	grid.resize(nb);
	double step =(max-min)/(nb-1);

	double x = min;
	for (int i=0; i<nb; i++, x+=step)
		grid[i] = x;
}

void explicit_df(
	const int		n,
	const double	dx,
	const dvector&	f,	
	const dvector&	a,	
	const dvector&	b,
	const dvector&	c,
	dvector&		df
	)
{
	for (int i=1; i<n-1; i++)
		df[i]   = a[i]*f[i] + b[i]*(f[i+1]-f[i-1])/(2*dx) + c[i]*(f[i+1]-2*f[i]+f[i-1])/(dx*dx);
	int i=0; 
		df[0]   = a[i]*f[i] + b[i]*(f[i+1]-f[i])/dx       + c[i]*(f[i+2]-2*f[i+1]+f[i])/(dx*dx);
	i=n-1;
		df[n-1] = a[i]*f[i] + b[i]*(f[i]-f[i-1])/dx       + c[i]*(f[i]-2*f[i-1]+f[i-2])/(dx*dx);
}

void implicit_df_coefs(
	const int		n,
	const double	dx,
	const dvector&	a,	
	const dvector&	b,
	const dvector&	c,
	dvector&		diag,
	dvector&		upper_diag,
	dvector&		lower_diag
	)
{
	// indices of band diag vectors are row indices 
	// diag lower[0] and upper[n] are discarded.
	lower_diag[0] = upper_diag[n-1] = -999999999;	// NaN
	
	for (int i=1; i<n-1; i++) {
		diag[i]       =  a[i]      - 2*c[i]/(dx*dx);	// f[i] coef
		lower_diag[i] = -b[i]/(2*dx) + c[i]/(dx*dx);	// f[i-1] coef
		upper_diag[i] = +b[i]/(2*dx) + c[i]/(dx*dx);	// f[i-1] coef
	}

	// assume second derivative=0
	diag[0]           = a[0] - b[0]/dx;;
	upper_diag[0]     = b[0]/dx;

	diag[n-1]         = a[n-1] + b[n-1]/dx;
	lower_diag[n-1]   = -b[n-1]/dx;
}


void explicit_step(
	const dvector&	x,
	const double	dx,
	const dvector&	a,	
	const dvector&	b,
	const dvector&	c,
	const double	dt,
	const dvector&	f_t0,
	dvector&		f_t1,	///< can be a reference do F_t0 which will get modified
	dvector&		df_t0
	)
{
	int n=size(f_t0);
	resize_vec(df_t0, n);
	explicit_df(n, dx, f_t0, a, b, c, df_t0);

	for (int i=0; i<n; i++)
		f_t1[i] = f_t0[i] + df_t0[i]*dt;
}

void cn_step(
	const double	theta,
	const dvector&	x,
	const double	dx,
	const dvector&	a,	
	const dvector&	b,
	const dvector&	c,
	const double	dt,
	const dvector&	f_t0,
	dvector&		f_t1	///< can be a reference do F_t0 which will get modified
	)
{
	int n=size(f_t0);
	dvector df_t0(n);
	dvector df_t1_diag(n), df_t1_upper_diag(n), df_t1_lower_diag(n);
	
	explicit_df(n, dx, f_t0, a, b, c, df_t0);
	implicit_df_coefs(n, dx, a, b, c, df_t1_diag, df_t1_upper_diag, df_t1_lower_diag);

	// [L^1-1/(theta*dt)](f_1) =  - [ f_0/(theta*dt) + (1-theta)/theta.L^0(f_0) ]
	for (int i=0; i<n; i++) {
		df_t1_diag[i] -= 1./(theta*dt);
		df_t0[i] = - ( f_t0[i]/(theta*dt) + (1-theta)/theta*df_t0[i] ); 
		
	}

	tridiag_solve(df_t1_lower_diag, df_t1_diag, df_t1_upper_diag, f_t0, f_t1);
}

