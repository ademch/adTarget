#pragma once
#include <base/typedefs.h>
struct pde_memory_1d;

/// Kolmogorov backward:		f(t,x) := E[ u(X_T) | X_t = x ]    for t<T
///	with final condition:	    f(T,x)  = u(x) 
/// with diffusion:				X_t	:= mu.dt + sigma.dW_t  where mu(t,X_t) and sigma(t,X_t)
///
/// Using Ito lemma:			df(t,X_t) = [ mu.df/dx + 1/2.d2f/dx2.sigma^2 ].dt + sigma.df/dx dW_t
///					
/// f(t,x) = E[ F(X_T) | X_t = x ]							(definition of f)
///			= E[ E[ F(X_T) | X_t_plus_1=y ] | X_t = x ]		(conditional esperance)
///			= E[ f(t_plus_1,X_t_plus_1) | X_t = x ]			(definition of f)
///			= f(t_plus_1,x) - mu.df/dx - 1/2.d2f/dx2.var	(Ito lemma from above)
///
/// Then
/// -df/dt(t,x) = mu(t,x) . df/dx(t,x) + 1/2.sigma^2(t,x).d2f/dx2
///
/// Wikipedia: 
/// KBE (1931) was developed before and can be derived from Feynman-Kac formula (1949).

/// exported : no
void pde_backward_step_1d(
	const int		n,				///< NbSpacePts 
	const double	dt,				///< time step
	const double	cn,				///< Crank-Nicholson theta
	const double*	x,				///< [NbSpacePts] values of X_t
	const double*	mu,				///< [NbSpacePts] mu(t+1,x)
	const double*	sigma2,			///< [NbSpacePts] sigma^2(t+1,x)
	const double*	f_t_plus_1,		///< [NbSpacePts] f(t+1,x) = E[ u(X_T) | X_{t+1} = x]
	double* 		f_bwd_step,		///< [NbSpacePts] f(t,x) - f(t+1,x)
	pde_memory_1d&	mem				///< internal memory owner
	);

/// Kolmogorov forward:			f(t,x) := E[ u(X_t) | t_0 ]
/// with initial condition:		f(0,x)  = u(x)	
/// with diffusion:				X_t	:= mu.dt + sigma.dW_t  where mu(t,X_t) and sigma(t,X_t)
/// 
/// df/dt(t,x) = -d/dx( mu(t,x).f(t,x) ) + 1/2.d2/dx2( sigma^2(t,x).f(t,x) )
///
/// Wikipedia: 
/// KFE (1931) came after and can be derived from Fokker–Planck equation.

/// exported : no
void pde_forward_step_1d(
	const int		n,				///< NbSpacePts 
	const double	dt,				///< time step
	const double	cn,				///< Crank-Nicholson theta
	const double*	x,				///< [NbSpacePts] values of X_t
	const double*	mu,				///< [NbSpacePts] mu(t+1,x)
	const double*	sigma2,			///< [NbSpacePts] sigma^2(t+1,x)
	const double*	f,				///< [NbSpacePts] f(t,x) = E[ u(X_t) | t_0 ]
	double* 		f_fwd_step,		///< [NbSpacePts] f(t+1,x) - f(t,x)
	pde_memory_1d&	mem				///< internal memory owner
	);


/// Test function.
void pde_step_1d_test(
	const bool		isBackward,		
	const bool		isLogNormal,
	const int		n,				///< NbSpacePts 
	const double	cn,				///< Crank-Nicholson theta
	const dvector&	x,	
	const double	dt,
	const double	mu,	
	const double	sigma2,
	const dvector&	f_in,			///< f(X)
	// output
	dvector&		f_out,			///< can be a reference do f_in which will get modified
	dvector&		c0,
	dvector&		c1,
	dvector&		c2,
	dvector&		r
	);

// memory used internally should be allocated externally
struct pde_memory_1d {
	pde_memory_1d(int n);

	dvector c0, c1, c2;
	dvector upper, diag, lower;
	dvector r;
};
