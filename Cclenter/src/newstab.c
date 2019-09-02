#ifndef NEWSTAB_C
#define NEWSTAB_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

void newstab()
{
    short v0;

    if ( no_stabs )
        v0 = no_stabs + 1;
    else
        v0 = 0;
    stab = (STAB **)mrealloc(stab, 4 * v0, 4 * no_stabs++ + 8);
}

#endif
