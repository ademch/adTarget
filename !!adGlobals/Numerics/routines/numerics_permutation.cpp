#include "numerics_permutation.h"
#include "numerics_special_fcts.h"
#include "numerics_linalg.h"
#include "base/typedefs.h"

double random_unif_01(int& seed)
{
	return NR::ran2(seed);
}

int random_int(int from_0_to_n_excluded, int& seed)
{
	return (int)(from_0_to_n_excluded * random_unif_01(seed));
}

void random_permutations(
	dvector&			initial_vect,			///< [vect_size] vector to be shuffled
	const uint			nb_perturbations,		
	int&				seed,
	dmatrix&			permuted_vectors )		///< [nb_perturbations][vect_size]
{
	const uint			vect_size = size(initial_vect);

	for (uint p=0; p<nb_perturbations; p++)
	{
		for (uint i=0; i<vect_size-1; i++)
		{
			// pick any value after us (because value already used are swapped to be before us)
			// between i and vect_size-1
			int rnd = i + random_int(vect_size-i, seed);

			permuted_vectors[p][i] = initial_vect[rnd]; 
			swap(initial_vect[i], initial_vect[rnd]);
		}

		// last item default to last item left (the only that has not be used yet)
		permuted_vectors[p][vect_size-1] = initial_vect[vect_size-1];
	}
}

// 	Generate fractiles
extern const char* enum_str_tirage_type[] = { "UNIFORM", "NORMAL" };
extern long enum_val_tirage_type[] = { UNIFORM, NORMAL };

void tirage_permuted_fractiles_indept(
	uint				numSimulation,	
	uint				numDimension, 
	tirage_type			type,
	int&				seed,
	dmatrix&			matrix)			///< [numDim][numSimul]
{
	double	step, proba;
	int		odd = 0;
	
	dvector init_gauss(numSimulation);
	
	step = 1.0 / (numSimulation+1);
	proba = step;

	if(numSimulation & 1) // odd
		odd = 1;
	numSimulation -= odd;
	numSimulation /= 2;

	// Generation of the fractiles of the gaussian */

	if(type==UNIFORM)
	{
		for (uint i=0; i<numSimulation; i++)
		{
			init_gauss[i] = proba;
			init_gauss[numSimulation+i+odd] = 1.0-proba;
			proba += step;
		}
	}
	else if (type==NORMAL)
	{
		for (uint i=0; i<numSimulation; i++)
		{
			init_gauss[i] = normal_cumulative_inv(proba);
			init_gauss[numSimulation+i+odd] = -init_gauss[i];
			proba += step;
		}
	}
	else
	{	
		throw std::runtime_error("tirage_Balsam_indept: Unknown type of distribution");
	}

	if(odd && type==0)
		init_gauss[numSimulation] = 0.5;
	else if(odd && type!=0)
		init_gauss[numSimulation] = 0;


	numSimulation *= 2;
	numSimulation += odd;

	random_permutations(
		init_gauss,			///< initial vector
		numDimension,		///< nb_perturbations
		seed,				///< seed (modified)
		matrix);			///< [nb_perturbations][vect_size]
}

/// Generates random variable with Balsam algorithm
void tirage_permuted_fractiles_correlated_gaussians(
	uint			numSimulation,		///< Number of simulation on each dimension
	uint			numDimension,		///< Number of dimension
	const dmatrix&	corr_mtx,			///< [numDim][numDim] Correlation matrix
	dmatrix&		gauss_mtx)			///< Output: [numDim][numSimul] MUST BE ALLOCATED Gaussian vector correlated realisations
{
	/// Cholesky decomposition

	dmatrix chol_dec(numDimension,numDimension);
	for(uint i=0; i<numDimension; i++)
		for(uint j=0; j<numDimension; j++)
			chol_dec[i][j] = corr_mtx[i][j];

	///< arg is in/out: input is a symmetric definite positive matrix (from upper triangle), output is lower triangle
	cholesky_decomp(chol_dec);		

	// Independent fractiles permuted
	// we call the random generator once with a negative value to initialize it.
	int seed = -123456789;
	tirage_permuted_fractiles_indept(
		numSimulation,
		numDimension, 
		NORMAL,
		seed,
		gauss_mtx);	///< [numDim][numSimul]

	// correlated Gaussians

	dvector	gau_idpt_vec(numDimension);
	dvector	gau_corr_vec(numDimension);
	
	for(uint iSim=0; iSim<numSimulation; iSim++)
	{
		for(uint i=0; i<numDimension; i++)
			gau_idpt_vec[i] = gauss_mtx[i][iSim];

		for(uint i=0; i<numDimension; i++) {
			double gau_corr = 0.0;
			for(uint j=0; j<numDimension; j++)
				gau_corr += chol_dec[i][j]*gau_idpt_vec[j];
			gau_corr_vec[i] = gau_corr;
		}

		for(uint i=0; i<numDimension; i++)
			gauss_mtx[i][iSim] = gau_corr_vec[i];
	}
}

void tirage_permuted_fractiles_correlated_gaussians_cumulative(
	uint			numSimulation,		///< Number of simulation on each dimension
	uint			numDimension,		///< Number of dimension
	const dmatrix&	gauss_mtx,			///< [numDim][numDim] Gaussian realisation
	dmatrix&		unif_mtx)			///< Output: [numDim][numSimul] MUST BE ALLOCATED cumulative of Gaussian realisations
{
	for(uint iSim=0; iSim<numSimulation; iSim++)
		for(uint iDim=0; iDim<numDimension; iDim++)
			unif_mtx[iDim][iSim] = normal_cumulative(gauss_mtx[iDim][iSim]);
}

void tirage_permuted_fractiles_indep_unif(
	uint			numSimulation,		///< Number of simulation on each dimension
	uint			numDimension,		///< Number of dimension
	dmatrix&		unif_mtx)			///< Output: [numDim][numSimul] MUST BE ALLOCATED cumulative of Gaussian realisations
{
	int seed = -123456789;
	tirage_permuted_fractiles_indept(
		numSimulation,
		numDimension, 
		UNIFORM,
		seed,
		unif_mtx);	///< [numDim][numSimul]
}