#ifndef MSTOXI_C
#define MSTOXI_C

#include "DBdefs.h"
#include "cl4.h"

int mstoxi(int *a1)
{
	unsigned char *v1;
	v1 = (unsigned char*)a1;
	return (*(v1 + 2) | (*(v1 + 1) << 8)) + (*v1 << 16);
}

/*
0x01234567	->

mstoxi starts: 
BFFFF30C = 67 45 23 01  gE#.
mstoxi returned x00674523
BFFFF308 = 23 45 67 00  #Eg.

Lmstoxi returned x00674523
*/

#endif



