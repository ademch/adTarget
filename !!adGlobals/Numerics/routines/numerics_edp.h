
void make_grid_1d_min_max_nb(
	double min, 
	double max, 
	int nb, 
	dvector& grid
	);

void explicit_df(
	const int		n,
	const double	dx,
	const dvector&	f,	
	const dvector&	a,	
	const dvector&	b,
	const dvector&	c,
	dvector&		df
	);

void implicit_df_coefs(
	const int		n,
	const double	dx,
	const dvector&	a,	
	const dvector&	b,
	const dvector&	c,
	dvector&		diag,
	dvector&		upper_diag,
	dvector&		lower_diag
	);

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
	);

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
	);