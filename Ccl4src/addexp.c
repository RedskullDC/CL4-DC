#ifndef ADDEXP_C
#define ADDEXP_C

#include <stdio.h>
#include <math.h>		// for exp functions stuff
#include "DBdefs.h"
#include "cl4.h"

double _addexp(double a1, int a2)
{
	double v2;
	double result;
	int exponent;
	
	//printf("_addexp(a1 = %G, a2 = %d)\n",a1,a2);

	if ( a1 == 0.0 )
		return a1;
	else
	{
		v2 = frexp(a1, &exponent);		// split exponent off, save in "exponent"
		exponent += a2;
		result = ldexp(v2, exponent);	// v3 = v2 * 2^exponent
	}
	//printf("_addexp: returning %G\n",result);
	return result;
}

#endif
