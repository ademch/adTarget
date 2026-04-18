#include "nr.h"

/// Solves the set of n linear equations R.x = b, 
/// where R is an upper triangular matrix stored in a and d. 
/// a[1..n][1..n] and d[1..n] are input as the output of the routine qrdcmp and are not modified. 
/// b[1..n] is input as the right-hand side vector, and is overwritten with the solution vector on output.
void NR::rsolv(Mat_I_DP &a, Vec_I_DP &d, Vec_IO_DP &b)
{
	int i,j;
	DP sum;

	int n=a.nrows();
	b[n-1] /= d[n-1];
	for (i=n-2;i>=0;i--) {
		for (sum=0.0,j=i+1;j<n;j++) sum += a[i][j]*b[j];
		b[i]=(b[i]-sum)/d[i];
	}
}
