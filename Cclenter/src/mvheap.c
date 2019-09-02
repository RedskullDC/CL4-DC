#ifndef MVHEAP_C
#define MVHEAP_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

short _mvheap(REFR *refr, short a2, short a3)
{
    REFR *v4;
    short result; // ax@5
    REFR *rfr;

    v4 = refr;
    for ( rfr = &refr[a3]; v4 < &refr[a2] && a2 < a3; --a3 )
    {
        _mvref(v4++, --rfr);
    }
    
	result = a3;
    if ( a2 == a3 )
        result = v4 - refr;		// pointer arithmetic here
    return result;
}



#endif
