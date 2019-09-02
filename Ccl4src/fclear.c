#ifndef CDBFCLEAR_C
#define CDBFCLEAR_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

int cdbfclear(char *Buff, short TDFsize, short TDFtype)
{
    char *v3;
	short v4;
	
	//printf("cdbfclear(%08X, %2d, %02X) ",Buff, TDFsize, TDFtype );

	v3 = Buff;		// default string behaviour.
    *v3++ = 0;		// Zap first byte, and bump pointer
    if ( !(TDFtype & tdf_STRING) )	
    {
        for ( v4 = TDFsize - 1 ; v4 > 0 ; --v4)		// If not a string variable, clear the rest of it.
			*v3++ = 0;
    }
    return v3 - Buff; // return number of bytes cleared
}

#endif
