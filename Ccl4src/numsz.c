#ifndef NUMSIZE_C
#define NUMSIZE_C

#include "DBdefs.h"
#include "cl4.h"

short numsz(char *a1, unsigned int MaxLen)
{
	short i; // ecx@1

	//printf("numsz(a1: x%08X ,MaxLen: %d )\n",a1,MaxLen);
	//DumpBlock(a1,MaxLen);

	//v2 = a1;
	//for ( i = 0; i < MaxLen && (*a1 - 48) <= 9u; a1++ )	// <- gcc opt produces this
	for ( i = 0; i < MaxLen && (*a1 >= '0' && *a1 <= '9'); a1++ )
		i++;
	return i;
}
#endif
