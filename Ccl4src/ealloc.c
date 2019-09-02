#ifndef EALLOC_C
#define EALLOC_C

#include "DBdefs.h"
#include "cl4.h"

void* ealloc(int size,void* a2)
{
	void *result; // eax@1
	
	result = nalloc(size, a2);
	if ( !result )
		derror(43, 0, 0);
	return result;
}
#endif
