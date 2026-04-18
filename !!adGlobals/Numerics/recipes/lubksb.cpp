#include "nr.h"

/// Solves the set of n linear equations A . X = B. 
/// Here a[1..n][1..n] is input, not as the matrix A but rather as its LU decomposition, determined by the routine ludcmp. 
/// indx[1..n] is input as the permutation vector returned by ludcmp. 
/// b[1..n] is input as the right-hand side vector B, and  returns with the solution vector X. 
/// a, n, and indx are not modified by this routine and can be left in place for successive calls with different right-hand sides b. 
/// This routine takes into account the possibility that b will begin with many zero elements, so it is effcient for use in matrix inversion.

void NR::lubksb(Mat_I_DP &a, Vec_I_INT &indx, Vec_IO_DP &b)
{
	int i,ii=0,ip,j;
	DP sum;

	int n=a.nrows();
	for (i=0;i<n;i++) {
		ip=indx[i];
		sum=b[ip];
		b[ip]=b[i];
		if (ii != 0)
			for (j=ii-1;j<i;j++) sum -= a[i][j]*b[j];
		else if (sum != 0.0)
			ii=i+1;
		b[i]=sum;
	}
	for (i=n-1;i>=0;i--) {
		sum=b[i];
		for (j=i+1;j<n;j++) sum -= a[i][j]*b[j];
		b[i]=sum/a[i][i];
	}
}

/// Matrix inversion using LU decomposition

Mat_DP luinv(Mat_I_DP& mat) {
	int N = mat.nrows();
	Mat_DP out = Mat_DP(0.0, N, N);
	Vec_INT indx(0, N);
	Mat_DP tmp = mat;
	double d;
	int i,j;
 	Vec_DP col(N);

	NR::ludcmp(tmp, indx, d); // LU decomposition.
 	for(j=0; j<N; j++) { 
		// find inverse by columns.
		for(i=0; i<N; i++) 
			col[i]=0.0;
		col[j]=1.0;
		
		NR::lubksb(tmp, indx, col);
		for(i=0; i<N; i++) 
			out[i][j]=col[i];
 	}

	return out;
}
