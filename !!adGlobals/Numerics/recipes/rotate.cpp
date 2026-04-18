#include <cmath>
#include "nr.h"
using namespace std;


/// Given matrices r[1..n][1..n] and qt[1..n][1..n], 
/// carry out a Jacobi rotation on rows i and i+1 of each matrix. 
/// a and b are the parameters of the rotation: 
/// cos(theta) = a / sqrt(a^2+b^2) ,
/// sin(theta) = b / aqrt(a^2+b^2) .
void NR::rotate(Mat_IO_DP &r, Mat_IO_DP &qt, const int i, const DP a,
	const DP b)
{
	int j;
	DP c,fact,s,w,y;

	int n=r.nrows();
	// Avoid unnecessary overflow or underflow.
	if (a == 0.0) {
		c=0.0;
		s=(b >= 0.0 ? 1.0 : -1.0);
	} else if (fabs(a) > fabs(b)) {
		fact=b/a;
		c=SIGN(1.0/sqrt(1.0+(fact*fact)),a);
		s=fact*c;
	} else {
		fact=a/b;
		s=SIGN(1.0/sqrt(1.0+(fact*fact)),b);
		c=fact*s;
	}
	for (j=i;j<n;j++) {
		y=r[i][j];
		w=r[i+1][j];
		r[i][j]=c*y-s*w;
		r[i+1][j]=s*y+c*w;
	}
	for (j=0;j<n;j++) {
		y=qt[i][j];
		w=qt[i+1][j];
		qt[i][j]=c*y-s*w;
		qt[i+1][j]=s*y+c*w;
	}
}
