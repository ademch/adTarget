#include <vector>
#include <cmath>
#include <cassert>
#include <iostream>
#include <string>
using namespace std;

typedef size_t	uint;
typedef vector<double>	dvector;

inline
double sign(double a) {
	return a<0 ? -1. : 1.;
}

//////////////////////////////////////////////////////////////////////////

struct cmplx { 
	cmplx(double r=0., double i=0.) : r(r), i(i) {}
	double r, i; 
};
void complex_2_polar(
	cmplx	a,
	double&	n,
	double&	t)
{
	n = sqrt(a.r*a.r+a.i*a.i);
	t = acos(a.r/n)*sign(a.i);	///< possible div by zero
}
cmplx complex_cartesian(
	double	n,
	double	t)
{
	cmplx	c;
	c.r = n*cos(t);
	c.i = n*sin(t);
	return c;
}
cmplx complex_product(
	cmplx	a,
	cmplx	b)
{
	cmplx	c;
	c.r = a.r*b.r - a.i*b.i;
	c.i = a.r*b.i + a.i*b.r;
	return c;
}
cmplx complex_inverse(cmplx a)
{
	cmplx	c;
	double	n,t;
	complex_2_polar(a,n,t);
	return complex_cartesian(1./n,-t);
}
cmplx complex_sqrt(cmplx a)
{
	cmplx	c;
	double	n,t;
	complex_2_polar(a,n,t);
	if (t!=t)
		// theta=NaN : because division by norm(a) = 0  =>  we return sqrt(0)=0.
		return cmplx(0,0);	
	return complex_cartesian(sqrt(n),t/2);
}

cmplx operator+(const cmplx& a, const cmplx& b) { return cmplx(a.r+b.r, a.i+b.i); }
cmplx operator-(const cmplx& a, const cmplx& b) { return cmplx(a.r-b.r, a.i-b.i); }
cmplx operator-(const cmplx& b)					{ return cmplx(   -b.r,	   -b.i); }
cmplx operator*(const cmplx& a, const cmplx& b) { return complex_product(a,b); }

typedef std::vector<cmplx>	cvector;

//////////////////////////////////////////////////////////////////////////

dvector polynomial_roots_to_coefs(
	const dvector&	roots
	)
{
	uint n = roots.size();
	dvector res(n+1, 0.0);
	
	res[n] = 1.0;

	for (uint iRoot=0; iRoot<n; iRoot++) 
		for (uint iCoef=n-1-iRoot; iCoef<n; iCoef++) 
			res[iCoef] += -roots[iRoot] * res[iCoef+1];

	return res;
}

template <typename T>
T polynomial_product(
	const T&	coefs_lhs,
	const T&	coefs_rhs)
{
	uint n_lhs = coefs_lhs.size();
	uint n_rhs = coefs_rhs.size();
	T res((n_lhs-1)*(n_rhs-1)+1);
	
	for (uint iCoefLhs=0; iCoefLhs<n_lhs; iCoefLhs++) 
		for (uint iCoefRhs=0; iCoefRhs<n_rhs; iCoefRhs++) 
			res[iCoefLhs+iCoefRhs] = res[iCoefLhs+iCoefRhs] + coefs_lhs[iCoefLhs] * coefs_rhs[iCoefRhs];

	return res;
}
dvector polynomial_sum(
	const dvector&	coefs_lhs,
	const dvector&	coefs_rhs)
{
	uint n_lhs = coefs_lhs.size();
	uint n_rhs = coefs_rhs.size();
	uint n = max(n_lhs, n_rhs);
	dvector res(n, 0.0);
	
	for (uint iCoefLhs=0; iCoefLhs<n_lhs; iCoefLhs++) 
		res[iCoefLhs] += coefs_lhs[iCoefLhs];
	for (uint iCoefRhs=0; iCoefRhs<n_rhs; iCoefRhs++) 
		res[iCoefRhs] += coefs_rhs[iCoefRhs];

	return res;
}


dvector polynomial_real_roots_2nd_degree(
	const dvector&	coefs )
{
	assert(coefs.size()==3);
	double a = coefs[2];
	double b = coefs[1];
	double c = coefs[0];

	double delta = b*b-4*a*c;

	dvector res;
	if (delta>=0) {
		// two real roots
		res.push_back((-b-sqrt(delta))/(2*a));
		res.push_back((-b+sqrt(delta))/(2*a));
	}
	return res;
}


void polynomial_complex_roots_2nd_degree(
	const cmplx&	a, 
	const cmplx&	b, 
	const cmplx&	c,
	cmplx&			s1,
	cmplx&			s2)
{
	cmplx csqrtDelta = complex_sqrt(b*b - 4*a*c);

	s1 = (-b-csqrtDelta) * complex_inverse(2*a);
	s2 = (-b+csqrtDelta) * complex_inverse(2*a);
}

dvector polynomial_complex_coefs_real_roots_2nd_degree(
	const cmplx&	a, 
	const cmplx&	b, 
	const cmplx&	c)
{
	dvector res;

	cmplx s1, s2;
	polynomial_complex_roots_2nd_degree(a, b, c, s1, s2);

	const double eps = 1e-5;
	if (abs(s1.i)<eps) 
		res.push_back(s1.r);
	if (abs(s2.i)<eps)
		res.push_back(s2.r);

	return res;
}

	
dvector polynomial_real_roots_3rd_degree(
	const dvector&	coefs)
{
	assert(coefs.size()==4);
	
	// P(X) = X^3 + b.X^2 + c.X + d		after scaling by a
	double a = coefs[3];
	double b = coefs[2]/a;
	double c = coefs[1]/a;
	double d = coefs[0]/a; 
	a = 1.0;

	// P(X) = Q(X+b/3)
	// with Q(X) = X^3 + p.X + q
	double p = c - b*b/3;
	double q = d + b*b*b*2/27 - b*c/3;

	// with X = x + y
	//		Q(X) = x^3 + y^3 + (3xy+p)(x+y) + q
	// so if (x,y) solves system (S)
	//		3xy + p = 0
	//		x^3 + y^3 + q = 0
	// then x+y is root of Q

	// this is equivalent to 
	//		x^3 . y^3 = -p^3/27
	//		x^3 + y^3 = -q
	// this means x^3 and y^3 are roots of
	//		R(T) = T^2  + q.T - p^3/27
	// discriminant
	//		Delta = q^2 + 4*p^3/27
	//		D = 27.Delta

	double delta = q*q + p*p*p*4/27;
	double delta_explicit = ( 4*c*c*c + 27*d*d + 4*d*b*b*b - b*b*c*c - 18*b*c*d ) / 27;
	
	// real roots of (S) depends on sign of delta
	dvector res;
	const double eps = 1e-11;
	const double pi_times_2_over_3 = 2.*3.1415926535897932384626433832795/3.;
	
	if (delta>eps) {

		// R roots are real
		double t1 = -q-sqrt(delta)/2;
		double t2 = -q+sqrt(delta)/2;

		// (S) solutions are (x,y)
		// x = t1^(1/3) . j^k  with j=exp(i.2Pi/3) and k in {0,1,2}
		// idem y with t2
		// with constraint : xy (and x+y) must be real

		// Q has one real root and two conjugate non real root 
		// real root :				t1^(1/3)	+		t2^(1/3)
		// conjugate roots :	j^2.t1^(1/3)	+	j . t2^(1/3)
		// and					j . t1^(1/3)	+	j^2.t2^(1/3)
		double q_root = sign(t1)*exp(log(abs(t1))/3.) + sign(t2)*exp(log(abs(t2))/3.);
		
		res.push_back(q_root);

	} else if (delta<-eps) {

		// R roots are conjugates : t = -q +/- i.sqrt(-delta)/2   
		// or						t = norm . exp( +/- i.theta )
		
		cmplx t(-q,sqrt(-delta)/2);
		double norm_t, theta_t;
		complex_2_polar(t, norm_t, theta_t);

		// z1,z2 a cubic root of R roots
		double norm_z = exp(log(norm_t)/3);
		double theta_z = theta_t/3;

		// Q has three real roots :
		// 			z1	+		z2	=	norm . (exp(i.theta)+exp(i.(-theta)))				= 	norm_z . 2.cos(theta_z)
		// 		j^2.z1	+	j . z2	=	norm . (exp(i.(theta-2pi/3))+exp(i.(-theta+2pi/3))) = 	norm_z . 2.cos(theta_z-2pi/3)
		//		j . z1	+	j^2.z2	=	norm . (exp(i.(theta+2pi/3))+exp(i.(-theta-2pi/3))) = 	norm_z . 2.cos(theta_z+2pi/3)
		res.push_back(2.*norm_z*cos(theta_z));
		res.push_back(2.*norm_z*cos(theta_z+pi_times_2_over_3));
		res.push_back(2.*norm_z*cos(theta_z-pi_times_2_over_3));

	} else {

		// R has a double real root
		double t = -q/2;

		// Q has three real roots :
		// real root :				t^(1/3)		+		t^(1/3)		=	2.t^(1/3)
		// double root :		j^2.t^(1/3)		+	j . t^(1/3)		=	-t^(1/3)
	
		double q_half_no_root = sign(t)*exp(log(abs(t))/3.);
		res.push_back(2.*q_half_no_root);
		res.push_back(-q_half_no_root);
		res.push_back(-q_half_no_root);
	
	}

	// from Q roots to P roots
	for (size_t i=0; i<res.size(); i++)
		res[i] = res[i]-b/3;

	return res;
}

dvector polynomial_real_roots_4th_degree(
	const dvector&	coefs )
{
	assert(coefs.size()==5);
	
	// P(X) = X^4 + b.X^2 + c.X^2 + d.X + e		after scaling by a
	double a = coefs[4];
	double b = coefs[3]/a;
	double c = coefs[2]/a;
	double d = coefs[1]/a;
	double e = coefs[0]/a;
	a = 1.0;

	// P(X) = Q(X+b/4)
	// with Q(X) = X^4 + p.X^2 + q.X + r
	double mu = b/4;
	double p = c - b*b*3/8;
	double q = d + b*b*b/8	      - b*c/2;
	double r = e - b*b*b*b*3/256 + b*b*c*1/16 - b*d/4 ;

	// Q(X) = T(X)^2 + S(X)
	// with 
	// T(X) = X^2 + phi
	// S(X) = u.X^2 + v.X + w
	// => p=2.phi+u, q=v, r=phi^2+w
	// => u=p-2.phi, v=q, w=r-phi^2

	// Choose phi so that, S(X) is the square of a first degree polynom
	// eg S(X) = u.(X+g)^2
	// => 2ug=v and u.g^2=w
	// => constraint relation v^2 = 4uw 

	// replacing u,v,w in this relation 
	// => q^2 = 4.(p-2.phi).(r-phi^2)
	// phi should solve the trinom
	// (2.phi)^3 - p.(2.phi)^2 - 4r.2.phi + 4pr-q^2 = 0

	// phi0, a solution of this trinom
	dvector coefs3(4);
	coefs3[3] = 8;
	coefs3[2] = -4*p;
	coefs3[1] = -8*r;
	coefs3[0] = 4*p*r-q*q;
	dvector root3 = polynomial_real_roots_3rd_degree(coefs3);
	
	const double eps	= 1e-11;
	dvector	res;
	
	for (size_t i3=0; i3<root3.size(); i3++) {
		res.resize(0);

		double  phi0		= root3[i3];
		double  z_square 	= p - 2*phi0;
		// z : a solution of z^2 = u = p - 2.phi0

		if (abs(z_square)>eps) {

			// g = v/2u = b/(2.z^2)
			// Q(X) = (X^2 + phi0)^2 + (u.X^2 + v.X + w)
			// Q(X) = (X^2 + phi0)^2 + u.(X + g)^2				(with g = v/2u = q/2(p-2.phi) = q/(2.z^2) )
			// Q(X) = (X^2 + phi0)^2 + z^2.(X+q/(2.z^2))^2						(proof need)
			// Q(X) = (X^2 + iz.X + iz.g + phi0).(X^2 - iz.X - iz.g + phi0)		(factorization)

			double g = q/(2*z_square);

			// we have to solve to 2nd degree equation with complex coefficients			
			// if z^2 > 0,	z = sqrt(z^2)		=>  i.z = i.sqrt(z^2)
			// if z^2 < 0,	z = i.sqrt(|z^2|)	=>	i.z = -sqrt(|z^2|)
			
			dvector root2;
			cmplx iz = z_square<0 ? -sqrt(-z_square) : cmplx(0.0,sqrt(z_square));				
			
			// first 2nd degree equation

			root2 = polynomial_complex_coefs_real_roots_2nd_degree(
				1.0,									//	X^2
				iz,										// + iz.X
				complex_product(iz,g) + phi0 );			// + iq/2z + phi0

			for (size_t i=0; i<root2.size(); i++) 
				res.push_back(root2[i]);

			// second 2nd degree equation

			root2 = polynomial_complex_coefs_real_roots_2nd_degree(
				1.0,									//	X^2
				-iz,									// - iz.X
				-complex_product(iz,g) + phi0 );		// - iq/2z + phi0

			for (size_t i=0; i<root2.size(); i++) 
				res.push_back(root2[i]);

		} else {

			// z^2 = p-2phi0 = 0		=>	p = 2phi0
			// phi trinome becomes		=>  8.phi^3 - 8.phi0.phi^2 - 8.phi.r + 8.phi0.r - q^2 = 0
			// with phi=phi0			=>	q^2 = 0
			// Q(X) = X^4 + p.X^2 + r	=>	eg. Q(X) is bi-square and can easily be solved

			// check q << 1
			if (q*q>eps) 
				throw std::runtime_error("polynomial_real_roots_4th_degree: q should be << 1.");
			
			dvector coefs2(3);
			coefs2[2] = 1;
			coefs2[1] = p;
			coefs2[0] = r;
			dvector root2 = polynomial_complex_coefs_real_roots_2nd_degree(1, p, r);
			
			for (size_t i=0; i<root2.size(); i++) {
				if (root2[i]>=0.) {
					res.push_back(sqrt(root2[i]));
					res.push_back(-sqrt(root2[i]));
				}
			}
		}
		
		// from Q roots to P roots
		for (size_t i=0; i<res.size(); i++)
			res[i] = res[i]-b/4;
	}
	return res;
}

dvector polynomial_real_roots(
	const dvector&	coefs)
{
	uint n = coefs.size();
	const dvector&	a = coefs;

	dvector res;
	switch (n) {
		case 2:
			// 1st degree
			res.push_back(-a[0]/a[1]);
			break;
		case 3:
			// 2nd degree
			res = polynomial_real_roots_2nd_degree(coefs);
			break;
		case 4:
			// 3rd degree
			res = polynomial_real_roots_3rd_degree(coefs);
			break;
		case 5:
			// 4th degree
			res = polynomial_real_roots_4th_degree(coefs);
			break;
		default:
			throw "polynomial_roots: limited to 4th degree";
	}
	return res; 
}

//////////////////////////////////////////////////////////////////////////

void print(
		   const string&		txt,
		   const dvector&		v
		   )
{
	cout << txt << "\t";
	uint n = v.size();
	for (uint i=0; i<n; i++)
		cout << v[i] << "\t";
	cout << endl;
}

void print(
		   const string&		txt,
		   const cvector&		v
		   )
{
	cout << txt << "\t";
	uint n = v.size();
	for (uint i=0; i<n; i++)
		cout << "(" << v[i].r << "," << v[i].i << ")\t";
	cout << endl;
}

void main()
{
	dvector roots;
	dvector roots_dbl;
	roots_dbl.push_back(3.0);
	roots_dbl.push_back(3.0);

	dvector coefs_conjugate_roots;	// = (X^2+1).X
	coefs_conjugate_roots.push_back(0.0);
	coefs_conjugate_roots.push_back(1.0);
	coefs_conjugate_roots.push_back(0.0);
	coefs_conjugate_roots.push_back(1.0);

	
	roots.push_back(3.0);
	print("P(X) ", polynomial_roots_to_coefs(roots));
	print("roots", polynomial_real_roots(polynomial_roots_to_coefs(roots)));
	cout << endl;

	roots.push_back(5.0);
	print("P(X)", polynomial_roots_to_coefs(roots));
	print("roots", polynomial_real_roots(polynomial_roots_to_coefs(roots)));
	cout << endl;

	cout << " -- " << endl;
	cout << endl;

	// deg 3

	roots.push_back(7.0);
	print("roots_in", roots);
	print("P(X)", polynomial_roots_to_coefs(roots));
	print("roots_out", polynomial_real_roots(polynomial_roots_to_coefs(roots)));
	cout << endl;

	roots_dbl.push_back(7.0);
	print("roots_in", roots_dbl);
	print("P2(X) = (X-xi)^2.(X-xj)", polynomial_roots_to_coefs(roots_dbl));
	print("roots_out", polynomial_real_roots(polynomial_roots_to_coefs(roots_dbl)));
	cout << endl;

	cout << "roots_in i, -i, 0." << endl;
	print("Pconj(X) = (X^2+1).X = ", coefs_conjugate_roots);
	print("roots_pout", polynomial_real_roots(coefs_conjugate_roots));
	cout << endl;

	cout << " -- " << endl;
	cout << endl;

	// deg 4

	roots.push_back(7.0);
	print("roots_in", roots);
	print("P(X)", polynomial_roots_to_coefs(roots));
	print("roots_out", polynomial_real_roots(polynomial_roots_to_coefs(roots)));
	cout << endl;
	cout << " - " << endl;

	dvector roots_bisquare;
	roots_bisquare.push_back(1.0);
	roots_bisquare.push_back(1.0);
	roots_bisquare.push_back(2.0);
	roots_bisquare.push_back(2.0);
	print("roots_in", roots_bisquare);
	print("P(X) bisquare", polynomial_roots_to_coefs(roots_bisquare));
	print("roots_out", polynomial_real_roots(polynomial_roots_to_coefs(roots_bisquare)));
	cout << endl;
	cout << " - " << endl;

	dvector roots_bisquare2;
	roots_bisquare2.push_back(1.1);
	roots_bisquare2.push_back(1.1);
	roots_bisquare2.push_back(2.3);
	roots_bisquare2.push_back(2.3);
	print("roots_in", roots_bisquare2);
	print("P(X) bisquare", polynomial_roots_to_coefs(roots_bisquare2));
	print("roots_out", polynomial_real_roots(polynomial_roots_to_coefs(roots_bisquare2)));
	cout << endl;

	cout << " -- " << endl;
	cout << endl;

	int i;
	cin >> i;
}
