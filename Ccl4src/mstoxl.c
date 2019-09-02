#ifndef MSTOXL_C
#define MSTOXL_C

#include "DBdefs.h"
#include "cl4.h"

double mstoxl(int *a1)
{
	unsigned char *v1;

	//printf("mstoxl(%d)\n", *a1);

	v1 = (unsigned char*)a1;
	return (*(v1 + 5) | (*(v1 + 4) << 8)) + _addexp((*(v1 + 3) | (*(v1 + 2) << 8)), 16) + _addexp((*(v1 + 1) | (*v1 << 8)), 32);
}


#endif



