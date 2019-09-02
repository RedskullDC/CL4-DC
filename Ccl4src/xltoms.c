#ifndef XLTOMS_C
#define XLTOMS_C

#include <math.h>               // for exp functions stuff
#include "DBdefs.h"
#include "cl4.h"

long long* xltoms(long long *a1, double a2)
{
	double v2; // fst7@1
	double v3; // fst7@1
	double v4; // fst7@1
	double v5; // fst7@1
	double v6; // fst7@1
	double v7; // fst7@1
	double iptr;

	unsigned short *v1;
	v1 = (unsigned short *)a1;

	v2 = _addexp(a2, -16);
	v3 = modf(v2, &iptr);
	v4 = _addexp(v3, 16);
	itoms(v1 + 2, v4);
	v5 = _addexp(iptr, -16);
	v6 = modf(v5, &iptr);
	v7 = _addexp(v6, 16);
	itoms(v1 + 1, v7);
	itoms(v1, iptr);
	return a1;
}



#endif



