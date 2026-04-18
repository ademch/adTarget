#pragma once
#include "base/typedefs.h"

typedef unsigned int 	uint;

enum tirage_type { UNIFORM, NORMAL };
//extern const char* enum_str_tirage_type[];
//extern long enum_val_tirage_type[];

/// as the (long) name suggests
void tirage_permuted_fractiles_indept(
	uint				numSimulation,	
	uint				numDimension, 
	tirage_type			type,
	int&				seed,
	darray2&			matrix			///< [numDim][numSimul]
	);
