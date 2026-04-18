#include <cmath>
#include "nr.h"
using namespace std;

/// Given a positive-definite symmetric matrix a[1..n][1..n], 
/// this routine constructs its Cholesky decomposition, A = L.L'. 
/// On input, only the upper triangle of a need be given; it is not modified. 
/// The Cholesky factor L is returned in the lower triangle of a, 
/// except for its diagonal elements which are returned in p[1..n].
void NR::choldc(Mat_IO_DP &a, Vec_O_DP &p)
{
	if (p.size()==0) p = Vec_DP(a.nrows());

	int i,j,k;
	DP sum;

	int n=a.nrows();
	for (i=0;i<n;i++) {
		for (j=i;j<n;j++) {
			for (sum=a[i][j],k=i-1; k>=0; k--) 
				sum -= a[i][k]*a[j][k];
			if (i == j) {
				if (sum <= 0.0)
					nrerror("choldc failed");
				p[i]=sqrt(sum);
			} else a[j][i]=sum/p[i];
		}
	}
}

Mat_IO_DP& cholesky_decomp(Mat_IO_DP &a) {
	int n=a.nrows();
	Vec_DP p;
	NR::choldc(a, p);
	// uses p to fill the diagonal 
	for (int i=0; i<n; i++) 
		a[i][i] = p[i];
	// fill the upper triangle with zero
	for (int i=0; i<n; i++) 
		for (int j=i+1; j<n; j++) 
			a[i][j] = 0;
	return a;
}
