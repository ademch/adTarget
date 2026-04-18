#include "nr.h"

void NR::tridag(Vec_I_DP &a, Vec_I_DP &b, Vec_I_DP &c, Vec_I_DP &r, Vec_O_DP &u)
{
	int j;
	DP bet;

	int n=a.size();
	Vec_DP gam(n);
	if (b[0] == 0.0) nrerror("Error 1 in tridag");
	u[0]=r[0]/(bet=b[0]);
	for (j=1;j<n;j++) {
		gam[j]=c[j-1]/bet;
		bet=b[j]-a[j]*gam[j];
		if (bet == 0.0) nrerror("Error 2 in tridag");
		u[j]=(r[j]-a[j]*u[j-1])/bet;
	}
	for (j=(n-2);j>=0;j--)
		u[j] -= gam[j+1]*u[j+1];
}

/// Agnostic version.
/// Solves for a vector u[1..n] the tridiagonal linear set given by equation M.u=r. 
/// lower_diag[1] and upper_diag[n] are discarded. u must be allocated.
void tridag(
	unsigned int  n,
	const double* lower_diag, 
	const double* diag, 
	const double* upper_diag, 
	const double* r, 
	double*		  u);

void tridag(
	unsigned int  n,
	const double* a, 
	const double* b, 
	const double* c, 
	const double* r, 
	double* u)
{
	DP bet;

	Vec_DP gam(n);
	if (b[0] == 0.0) NR::nrerror("Error 1 in tridag");
	u[0]=r[0]/(bet=b[0]);
	for (unsigned int j=1;j<n;j++) {
		gam[j]=c[j-1]/bet;
		bet=b[j]-a[j]*gam[j];
		if (bet == 0.0) NR::nrerror("Error 2 in tridag");
		u[j]=(r[j]-a[j]*u[j-1])/bet;
	}
	for (int j=(n-2);j>=0;j--)
		u[j] -= gam[j+1]*u[j+1];
}
