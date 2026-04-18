#include "nr.h"

/// Solves the set of n linear equations A.x = b. 
/// a[1..n][1..n], c[1..n], and d[1..n] are input as the output of the routine qrdcmp and are not modified. 
/// b[1..n] is input as the right-hand side vector, and is overwritten with the solution vector on output.
void NR::qrsolv(Mat_I_DP &a, Vec_I_DP &c, Vec_I_DP &d, Vec_IO_DP &b)
{
	int i,j;
	DP sum,tau;

	int n=a.nrows();
	// Form Q'.b
	for (j=0;j<n-1;j++) {
		for (sum=0.0,i=j;i<n;i++) sum += a[i][j]*b[i];
		tau=sum/c[j];
		for (i=j;i<n;i++) b[i] -= tau*a[i][j];
	}
	// Solve R.x = Q'.b.
	rsolv(a,d,b);
}
