#ifndef RLIST_C
#define RLIST_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"	// for bit field values

RLARR *_rlist(RLIST *rlist, short count, int RecSize)
{
	RLARR	*rlarr;
	
    for ( rlarr = 0; count > 0; rlist++ )
    {
        //rlarr = (RLARR *)ealloc(RecSize + 36, rlarr);	// recsize + sizeof(RLARR)  chained structure
		// 36 on X86, 40 on X64

        rlarr = (RLARR *)ealloc(RecSize + sizeof(RLARR), rlarr);	// recsize + sizeof(RLARR)  chained structure

		// rlarr->NextRLARR is implicitly set to previous RLARR by ealloc();
        rlarr->PageNo			= rlist->PageNo;
        rlarr->Depth			= rlist->Depth;
        rlarr->field_1C			= 0;
        rlarr->Dpos.field_8		= 0;
        rlarr->Dpos.PageNo		= 0;
        rlarr->Dpos.NumEntries	= 0;
        rlarr->Dpos.field_4		= 0;
        --count;
    }
    return rlarr;
}

#endif
