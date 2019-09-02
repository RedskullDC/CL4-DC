#ifndef NEWITAB_C
#define NEWITAB_C

#include "DBdefs.h"
#include "cl4.h"

void newitab(void)
{
     ITAB	*v1;
	 int	v0;
	 unsigned int v2;
	 
     if ( no_itabs )
     {
          v0 = no_itabs + 1;
          v2 = it - itab;
     }
     else
     {
          v0 = 0;
     }
     v1 = (ITAB *)mrealloc(itab, sizeof(ITAB) * v0, sizeof(ITAB) * (no_itabs + 2));	// sizeof(ITAB) = 16
     itab = v1;
     if ( v0 )
          it = &v1[v2];
     else
          it = itab;
     ++no_itabs;
}

#endif
