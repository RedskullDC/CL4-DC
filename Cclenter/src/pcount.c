#ifndef PCOUNT_C
#define PCOUNT_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

/*
int pcount(int a1)		// not called in clenter
{
    int result; // eax@1
    int v2; // [sp+4h] [bp-14h]@2

    result = a1;
    if ( a1 == 19 )
    {
        v2 = 1;
    }
    else
    {
        if ( a1 != 20 )
            return result;
        v2 = 999;
    }
    return incvar(v_LCOUNT, v2);	// LCOUNT 19
}
*/
void chkeop()
{
    int plines;
	char a3[5];
	
    a3[0] = 0;
    plines = incvar(v_PLINES, 0);					// "PLINES" 21
    if ( plines )
    {
        if ( incvar(v_LCOUNT, 0) >= plines )		// "LCOUNT" 19
        {
            incvar(v_PCOUNT, 1);					// "PCOUNT" 20
            setvar(v_LCOUNT, 0);					// "LCOUNT" 19
            chkesc(0, 49, a3);						// bodgy handler tacked onto escape routines, on_eop handler
        }
    }
}


#endif
