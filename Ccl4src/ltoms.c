#ifndef LTOMS_C
#define LTOMS_C

#include <endian.h> // experimental swapping
#include "DBdefs.h"
#include "cl4.h"

// from /usr/include/bits/byteswap.h
//__bswap_constant_32(x)
//__bswap_constant_16(x)

int* ltoms(int* a1, int a2)                 // Long to Micro$oft conversion
{
	//*a1 = M_INT(a2);
	//printf("ltoms(a2 = x%08X) returns x%08X\n",a2,*(int*)a1);
	//return a1;

	//*a1 = __bswap_constant_32(a2);
	*a1 = __builtin_bswap32(a2);
	
	//printf("ltoms( a2 = x%08X, swapped == x%08X)\n" , a2, *a1);
	return a1;

} 

#endif
