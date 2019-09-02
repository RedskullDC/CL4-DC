#ifndef DTOMS_C
#define DTOMS_C

#include <math.h>               // for exp functions stuff
#include "DBdefs.h"
#include "cl4.h"

double *dtoms(double *a1, double a2)
{
	double v2; 
	double v3; 
	double v4; 
	double v5; 
	double v6; 
	double v7; 
	double v8; 
	double v9; 
	double v10;
	double iptr;

	unsigned short *v1;
	v1 = (unsigned short *)a1;

	v2 = _addexp(a2, -16);
	v3 = modf(v2, &iptr);
	v4 = _addexp(v3, 16);
	itoms(v1 + 3, v4);
	v5 = _addexp(iptr, -16);
	v6 = modf(v5, &iptr);
	v7 = _addexp(v6, 16);
	itoms(v1 + 2, v7);
	v8 = _addexp(iptr, -16);
	v9 = modf(v8, &iptr);
	v10 = _addexp(v9, 16);
	itoms(v1 + 1, v10);
	itoms(v1, iptr);
	return a1;
}
/*
BFFFF318 = 00 00 00 00 00 00 00 00 30 33 2F 30 32 2F 31 33  ........03/02/13
dtoms starts: 0

BFFFF318 = F4 FD 64 30 38 9B CD 41 30 33 2F 30 32 2F 31 33  ..d08..A03/02/13
dtoms starts: 9.93423E+08

dtoms returned 3.47784E+156 [x               0]

BFFFF310 = 00 00 00 00 3B 36 70 60 F4 FD 64 30 38 9B CD 41  ....;6p`..d08..A

BFFFF308 = 00 00 00 30 38 9B CD 41 00 00 00 00 3B 36 70 60  ...08..A....;6p`
mstod returned 9.93423E+08 [x        30000000]
*/

#endif

