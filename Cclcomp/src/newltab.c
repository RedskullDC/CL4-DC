#ifndef NEWLTAB_C
#define NEWLTAB_C

#include "DBdefs.h"
#include "cl4.h"

int newltab()
{
	int		v0;
	unsigned int v4;
	
	if ( no_ltabs )
	{
		v0 = no_ltabs + 1;
		v4 = lt - ltab;
	}
	else
		v0 = 0;

	ltab = (LTAB **)mrealloc(ltab, sizeof(LTAB*) * v0, sizeof(LTAB*) * (no_ltabs + 2));
	
	if ( v0 )
		lt = &ltab[v4];
	else
		lt = ltab;

	return ++no_ltabs;
}

#endif
