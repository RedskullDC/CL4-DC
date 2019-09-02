#ifndef LOADRESV_C
#define LOADRESV_C

#include <stdio.h>
#include <stdarg.h>				// for var args stuff
#include <ctype.h>				// for islower() and toupper() functions
#include <stdlib.h>				// for strtod()
#include "DBdefs.h"
#include "cl4.h"
#include "reserved.h"

short loadresv(char *Symbol)
{
	ReservedWord *rw_hi;
	ReservedWord *rw_lo;
	ReservedWord *rw_curr;
	short v3;
	 
	if ( *Symbol )
    {
		rw_lo = rvwords;
        //rw_hi = &rvwords[11];	// *** should be dynamically sized ***
        rw_hi = &rvwords[(sizeof(rvwords)/ sizeof(ReservedWord)) - 1];	// Pointer to last element in table

        while ( rw_lo <= rw_hi )
        {
			rw_curr = &rw_lo[(((rw_hi - rw_lo) >> 31) + rw_hi - rw_lo) >> 1];// simple tree search to speed things up
			v3 = strcmp(Symbol, rw_curr->Word);

			if (!v3)	// match!
				return (short)rw_curr->Rsv;
			else if ( v3 > 0 )
				rw_lo = rw_curr + 1;
			else	// v3 < 0
				rw_hi = rw_curr - 1;
		}
	}
    return 0;
}

#endif
