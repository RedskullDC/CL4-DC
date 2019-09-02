#ifndef NEWTTAB_C
#define NEWTTAB_C

#include "DBdefs.h"
#include "cl4.h"

void newttab(void)
{
	int v0; // edx@2
	//printf("newttab() - no_ttabs = %d, sizeof(TDesc) = %d \n",no_ttabs + 1,sizeof(TDesc) );
	if ( no_ttabs )
		v0 = no_ttabs + 1;
	else
		v0 = 0;
	ttab = (TDesc *)mrealloc(ttab, sizeof(TDesc) * v0, sizeof(TDesc) * (no_ttabs + 2));
	no_ttabs++;
}

#endif
