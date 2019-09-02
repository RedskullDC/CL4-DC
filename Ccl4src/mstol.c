#ifndef MSTOL_C
#define MSTOL_C

#include <endian.h> // experimental swapping
#include "DBdefs.h"
#include "cl4.h"

// from /usr/include/bits/byteswap.h
//__bswap_constant_32(x)
//__bswap_constant_16(x)

int mstol(int *a1)
{
	//unsigned char *LUC = (unsigned char *)a1;
	//printf("mstol( *a1 = x%08X, swapped == x%08X)\n" ,*a1, __bswap_constant_32(*a1));
	
	//return (unsigned int)(*(LUC + 3) | (*(LUC + 2) << 8)) + ((unsigned int)(*(LUC + 1) | (*LUC << 8)) << 16);

	return __bswap_constant_32(*a1);
}


#endif



