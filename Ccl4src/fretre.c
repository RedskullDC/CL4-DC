#ifndef FRETRE_C
#define FRETRE_C

#include <stdio.h>
#include "DBdefs.h"
#include "cl4.h"

void* fretre(void *ptr, void *a2)
{
	int	*intm;

	//printf("fretre( ptr: x%08X a2: x%08X)\n", ptr,a2);
	if ( ptr != a2 && ptr )
	{
		if ( *(int*)ptr )
			fretre((void*)*(int*)ptr, a2);
		intm = (int*)ptr;
		if ( (void*)intm[1] )
			fretre((void*)intm[1], a2);
		nfree(ptr, 0);
	}
	return a2;
}

#endif
