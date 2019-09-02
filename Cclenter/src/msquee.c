#ifndef MSQUEEZE_C
#define MSQUEEZE_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

short suck(char *Dest, char *Src, short Len)
{
    char *v4;

	for ( v4 = Dest ; Len > 0; --Len )
        *v4++ = *Src++;

	return (short)(v4 - Dest);		// return bytes copied + 1
}


short _msqueeze(REFR **a1, REFR **a2, REFR *a3, REFR *a4)
{
    REFR **v4;
	REFR **idx;
	REFR *v7;
	char *Dest;
	short bts;
	
    v4		= a1 - 1;
    Dest	= (*(a1 - 1))->Offset;
    
	for ( idx = a1 - 1; a2 < idx; --idx )
    {
        if ( *idx < a3 || a4 <= *idx )
        {
            if ( idx == v4 )
            {
                Dest = (*(idx - 1))->Offset;
            }
            else
            {
                bts = suck(Dest, (*idx)->Offset, (short)((*(idx - 1))->Offset - (*idx)->Offset) );
                *v4			= *idx;
                v7			= *idx;
                v7->Offset	= Dest;
                v7->Data	= v4;
                Dest += bts;
            }
            --v4;
        }
    }
    *v4			= *idx;
    v7			= *idx;
    v7->Offset	= Dest;
    v7->Data	= v4;
    return a1 - v4;           // return value is ignored by callers
}


#endif
