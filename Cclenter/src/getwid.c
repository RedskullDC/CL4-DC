#ifndef GETWIDTH_C
#define GETWIDTH_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <termcap.h>
#include <stdlib.h>		// for srand48()
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"	// for bit field values

short getwid(XTAB *xt)	// get width in chars required to display a field
{
	FLDdesc fld;
	
    if ( xt->WidthEXP )
        return evalint(&enarr.TableAddr[xt->WidthEXP - 1]);
    else
    {
        if ( xt->width == 0x400 )										// flag bit to get default width
			return getdwid(getftf(ENARR(xt->VarExpNo), 1, &fld));     // get defult width for this vartype
		else
			return xt->width;
    }
}

#endif
