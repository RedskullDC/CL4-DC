#ifndef HFLUSH_C
#define HFLUSH_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

void _hflush(SALLOCBUF *SAp, RLIST *rlst, TDinfo *TDptr)
{

	//printf("_hflush( SAp*: x%08X , rlst: x%08X, TDptr: x%08X\n", SAp, rlst, TDptr);
    while ( 1 )
    {
		//printf("_hflush - SAp->field_4 = %d\n",SAp->field_4); 
        _append(&SAp->refr, SAp->NodeArr, SAp->DataPtr, TDptr);
		if (! --SAp->field_4)	// Num recs in SA buff
			break;

        _mvref(SAp->DataPtr, &SAp->DataPtr[SAp->field_4]);
        _siftdown(SAp->DataPtr, SAp->field_4, 0, TDptr->TableDefs);
    }
    _pflush(SAp->NodeArr, &SAp->refr, rlst, TDptr);
}

#endif
