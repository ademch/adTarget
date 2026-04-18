#ifdef USE_NAG
#include "numerics_qp_nag.h"
#include <base/XptionC.h>
#include <nag/nag.h>			
#include <nag/nage04.h>			// nag_opt_qp - quadratic programming

#pragma comment(lib, "nagc.lib") 

void solve_quadprog_nag(
	const dmatrix&	H,			// [n][n|tdh>n] diagonal and upper triangular elements are referenced (remaining elements need not be assigned).
	const dvector&	c,			// [n]
	const dvector&	lx, 		// [n]
	const dvector&	ux, 		// [n] 
	const dmatrix&	A,			// [nclin][n|tda>n]  lines padding a[nclin][tda] with tda>=n 
	const dvector&	lA, 		// [nclin]
	const dvector&	uA, 		// [nclin]
	// output
	dvector			&x,
	double			&objf, 
	std::string		&msg
	)
{
	const char* fct = "solve_quadprog_nag";
	
	Nag_E04_Opt					options;
	nag_opt_init(&options);
	options.print_level			= Nag_NoPrint;
	options.minor_print_level	= Nag_NoPrint;
	options.list				= FALSE;
	options.print_deriv			= Nag_D_NoPrint;
	options.prob				= Nag_QP2;

	NagError					fail;
	INIT_FAIL(fail);
	fail.print					= FALSE;

	Nag_Comm					user_comm;

	Integer	n =	rows(H);		// the number of variables. Constraint: n > 0 .
	Integer	nclin = rows(A);	// the number of general linear constraints. nclin >= 0 
	Integer tda = n, tdh = n;

	require(size(c)==n,			"solve_quadprog_nag: c size mismatch.");
	require(size(lx)==n,		"solve_quadprog_nag: lx size mismatch.");
	require(size(ux)==n,		"solve_quadprog_nag: ux size mismatch.");
	require(size(lA)==nclin,	"solve_quadprog_nag: lA size mismatch.");
	require(size(uA)==nclin,	"solve_quadprog_nag: uA size mismatch.");

	resize_vec(x, n);
	
	dvector bl(n+nclin, 0.0);	// lower bounds 
	dvector bu(n+nclin, 1.0);	// upper bounds
	for (int i=0; i<n; i++) {
		// bounds on the variables
		bl[i] = lx[i]; 
		bu[i] = ux[i]; 
	}
	for (int i=0; i<nclin; i++) {
		// bounds for the general linear constraints
		bl[n+i] = lA[i]; 
		bu[n+i] = uA[i]; 
	}

	// remove constness
	double* a = const_cast<double*>(A[0]);
	double* h = const_cast<double*>(H[0]);
	double* cs = const_cast<double*>(&c[0]);

	nag_opt_qp(
		n,				
		nclin,

		a,					// [nclin,tda], general linear constraint factors
		tda,				// tda >= n
		
		&bl[0],				// [n+nclin] lower bounds 
		&bu[0],				// [n+nclin] upper bounds

		cs,					// cost vector
		h,					// quadratic H
		tdh,				// tdh >= n
		NULLFN,

		&x[0],				// [n] On entry:an initial estimate of the solution. On exit: the point at which nag_opt_qp terminated.
		&objf,				// On exit: the value of the objective function at x if x is feasible, or the sum of infeasibilities at x otherwise.

		&options,
		&user_comm,
		&fail);

	if (fail.code == NE_NOERROR)
		return;

	msg = fail.message; 
	if (fail.code == NW_DEAD_POINT || fail.code == NW_SOLN_NOT_UNIQUE || fail.code == NW_NOT_FEASIBLE) {
		warn_msg("%s: warning from nag : %s", fct, fail.message);
	} else {
		require(false, "%s: error in nag : %s", fct, fail.message);
	}
}

#endif USE_NAG