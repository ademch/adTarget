#include "nr.h"

// extern variables are defined in linmin.cpp
extern int ncom;
extern DP (*nrfunc)(Vec_I_DP &);
void (*nrdfun)(Vec_I_DP &, Vec_O_DP &);
extern Vec_DP *pcom_p,*xicom_p;

/// Given an n-dimensional point p[1..n] and an n-dimensional direction xi[1..n], 
/// moves and resets p to where the function func(p) takes on a minimum along the direction xi from p,
/// and replaces xi by the actual vector displacement that p was moved. 
/// Also returns as fret the value of func at the returned location p. 
/// This is actually all accomplished by calling the routines mnbrak and dbrent.

void NR::dlinmin(Vec_IO_DP &p, Vec_IO_DP &xi, DP &fret, DP func(Vec_I_DP &),
	void dfunc(Vec_I_DP &, Vec_O_DP &))
{
	const DP TOL=2.0e-8;
	int j;
	DP xx,xmin,fx,fb,fa,bx,ax;

	int n=p.size();
	ncom=n;
	pcom_p=new Vec_DP(n);
	xicom_p=new Vec_DP(n);
	nrfunc=func;
	nrdfun=dfunc;
	Vec_DP &pcom=*pcom_p,&xicom=*xicom_p;
	for (j=0;j<n;j++) {
		pcom[j]=p[j];
		xicom[j]=xi[j];
	}
	ax=0.0;
	xx=1.0;
	mnbrak(ax,xx,bx,fa,fx,fb,f1dim);
	fret=dbrent(ax,xx,bx,f1dim,df1dim,TOL,xmin);
	for (j=0;j<n;j++) {
		xi[j] *= xmin;
		p[j] += xi[j];
	}
	delete xicom_p;
	delete pcom_p;
}
