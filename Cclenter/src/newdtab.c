#ifndef NEWDTAB_C
#define NEWDTAB_C

#include "DBdefs.h"
#include "cl4.h"

void newdtab(void)
{
	int v0;

	// printf("newdtab() - no_dtabs = %d\n",no_dtabs + 1 );

	if ( no_dtabs )
		v0 = no_dtabs + 1;
	else
		v0 = 0;
	// dtab = ( DBase *)mrealloc(dtab, 12 * v0, 12 * (no_dtabs + 2));
	dtab = ( DBase *)mrealloc(dtab, sizeof(DBase) * v0, sizeof(DBase) * (no_dtabs + 2));
	no_dtabs++;
}

#endif
