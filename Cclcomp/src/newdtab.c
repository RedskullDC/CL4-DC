#ifndef NEWDTAB_C
#define NEWDTAB_C

#include "DBdefs.h"
#include "cl4.h"

void newdtab(void)
{
	int v0;
	
	//printf("newdtab() - no_dtabs = %d, sizeof(DBase) = %d\n",no_dtabs + 1, sizeof(DBase));
	
	if ( no_dtabs )
		v0 = no_dtabs + 1;
	else
		v0 = 0;
	dtab = ( DBase *)mrealloc(dtab, sizeof(DBase) * v0, sizeof(DBase) * (no_dtabs + 2));
	no_dtabs++;
}

#endif
