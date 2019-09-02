#ifndef COMPOSE_C
#define COMPOSE_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

// create temp tables

short _build(char *Buffer, int LDsize, short rtd, int *TTnos, short NUMTTs, EXPR *expr, bool Unique)
{
    TDinfo	*TDptr;
	short	result;
	short	v11;
	short	v12;
	short	v13;
	RLIST	rlarr[6];					// MaxDepth == 6
	
    TDptr = _chktd(rtd);
    if ( TDptr->TDSallocBuf )
        return -1;						// error, table already has been created?

    if ( Unique )
        TDptr->TDFlags |= tdp_UNIQUE;	// 0x800;		// compose == create unique
    else
        TDptr->TDFlags |= tdp_REORDER;	// 0x400;		// reorder = duplicates allowed, one-time pass

    TDptr->TDSallocBuf = _salloc((SALLOCBUF *)Buffer, LDsize, (unsigned int)(TDptr->TDRecMin + TDptr->TDRecSize) >> 1);
    v13 = join(rtd, TTnos, NUMTTs, expr);
    
	if ( Unique )
    {
        v11 = _commit(rtd, false);		// false means don't free the TDptr->SallocBuf mem
    }
    else
    {
        v12 = _sflush(rlarr, 6, TDptr->TDSallocBuf, TDptr);		// RLIST array has to be at least [6]  (6 = MaxDepth)
        v11 = v12;
        if ( v12 )
        {
            TDptr->TDSallocBuf = (SALLOCBUF *)_rlist(rlarr, v12, TDptr->TDRecSize);	//	_rlist returns an RLARR structure!!!
            TDptr->TDFlags |= 0x1002u;		//	flags indicate what type of structure is held in TDSallocBuf
            goto LABEL_11;
        }
    }
    TDptr->TDSallocBuf = 0;

LABEL_11:
    result = v13;
    if ( v13 >= 0 )
        result = v11;
    return result;
}

short compose(char *Buffer, int LDsize, short rtd, int *TTnos, short NumTTs, EXPR *expr)
{
	//return xenter((int)build, Buffer, LDsize, rtd, TTnos, NumTTs, expr, 1);
	return _build(Buffer, LDsize, rtd, TTnos, NumTTs, expr, 1);

}

short reorder(char *Buffer, int LDsize, short rtd, int *TTnos, short NumTTs, EXPR *expr)
{
	//return xenter((int)build, Buffer, LDsize, rtd, TTnos, NumTTs, expr, 0);
	return _build(Buffer, LDsize, rtd, TTnos, NumTTs, expr, 0);

}

#endif
