#include "nr.h"

/// Solves the set of n linear equations A.x = b, whereais a positive-definite symmetric matrix.
/// a[1..n][1..n] and p[1..n] are input as the output of the routine choldc. 
/// Only the lower triangle of a is accessed. 
/// b[1..n] is input as the right-hand side vector. 
/// The solution vector is returned in x[1..n]. 
/// a, n, and p are not modified and can be left in place for successive calls with different right-hand sides b.
/// b is not modified unless you identify b and x in the calling sequence, which is allowed.
void NR::cholsl(Mat_I_DP &a, Vec_I_DP &p, Vec_I_DP &b, Vec_O_DP &x)
{
	int i,k;
	DP sum;

	int n=a.nrows();
	for (i=0;i<n;i++) {
		for (sum=b[i],k=i-1;k>=0;k--) sum -= a[i][k]*x[k];
		x[i]=sum/p[i];
	}
	for (i=n-1;i>=0;i--) {
		for (sum=x[i],k=i+1;k<n;k++) sum -= a[k][i]*x[k];
		x[i]=sum/p[i];
	}
}
