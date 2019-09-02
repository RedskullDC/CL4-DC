#ifndef MSTOI_C
#define MSTOI_C

#include <endian.h> // experimental swapping
#include "DBdefs.h"
#include "cl4.h"

// from /usr/include/bits/byteswap.h
//__bswap_constant_32(x)
//__bswap_constant_16(x)

short mstoi(short *a1)
{
	//return *(a1 + 1) + (*a1 << 8);

	//printf("mstoi( *a1 = x%04X, returns : x%04X)\n" ,*a1,SWAP_SHORT(a1));
	//printf("mstoi( *a1 = x%04X, swapped == x%04X)\n" ,*a1, __bswap_constant_16(*a1));

	//return SWAP_SHORT(a1);

	return __bswap_constant_16(*a1);
}

#endif

