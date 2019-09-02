#ifndef FREELST_C
#define FREELST_C

#include "DBdefs.h"
#include "cl4.h"

void* frelst(void *ptr, void *a2)
{
	//printf("frelst(x%08X,x%08X)\n" ,ptr,a2);
	while ( ptr && ptr != a2 )
		ptr = nfree(ptr, *(char**)ptr);  // stop defreferencing nonsense

	return ptr;
}


#endif
