#include "nr.h"

/// Solves A.X=B for a vector X, where A is specified bt the arrays u[1..m][1..n], w[1..n], v[1..n][1..n] 
/// as returned by svdcmp. m and n are the dimensions of A, and will be equal for square matrices. 
/// b[1..m] is the input right-hand side. x[1..n] is the output solution vector.
/// No input quantities are destroyed, so the routine may be called sequentially with dierent b's.

void NR::svbksb(Mat_I_DP &u, Vec_I_DP &w, Mat_I_DP &v, Vec_I_DP &b, Vec_O_DP &x)
{
	int jj,j,i;
	DP s;

	int m=u.nrows();
	int n=u.ncols();
	Vec_DP tmp(n);
	for (j=0;j<n;j++) {
		s=0.0;
		if (w[j] != 0.0) {
			for (i=0;i<m;i++) s += u[i][j]*b[i];
			s /= w[j];
		}
		tmp[j]=s;
	}
	for (j=0;j<n;j++) {
		s=0.0;
		for (jj=0;jj<n;jj++) s += v[j][jj]*tmp[jj];
		x[j]=s;
	}
}
