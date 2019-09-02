#ifndef NEWBTAB_C
#define NEWBTAB_C

#include "DBdefs.h"
#include "cl4.h"
int newbtab()
{
     int v0;
     if ( no_btabs )
          v0 = no_btabs + 1;
     else
          v0 = 0;
     btab = (BTAB *)mrealloc(btab, sizeof(BTAB) * v0, sizeof(BTAB) * (no_btabs + 2));	// sizeof(BTAB) = 72 ; null terminated list
     return ++no_btabs;
}

#endif
