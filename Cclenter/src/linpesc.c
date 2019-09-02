#ifndef LINPUTESC_C
#define LINPUTESC_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

int linpesc(int KeyNumber, ONESC *esc)
{
    int result; // eax@2

    if ( !esc || (result = isesc(esc, KeyNumber)) == 0 )	// Field specific escape
        result = isesc(oelist, KeyNumber);					// look for global escape in this block
    return result;
}

int isesc(ONESC *esc, int KeyNumber)
{
//printf("isesc(x%08X,%d)\n", esc, KeyNumber);
    while ( esc )
    {
		if ( esc->OESCFlags && esc->KeyNumber == KeyNumber)
			return KeyNumber;
		esc = esc->NextESC;
    }
    return 0;	// not found
}

#endif
