#ifndef GETADWID_C
#define GETADWID_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>         // for strtol()
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"

XTAB *getadwid(XTAB *xtab)
{
	FLDdesc *fld;
	int		TDno;
	short	ExpNo;
	short	fno;
	char	FLDtype;

	//eprint("getadwid - symbol = %d\n",symbol);
	if ( symbol == 930 || symbol == 50 )		// <CR> or '+'
    {
		if ( !xtab->VarExpNo )
        {
			loaderr(37, sym);	// "invalid field width"
            return 0;
		}
        fno = gettf(xtab->VarExpNo, &TDno, &FLDtype);
		assert(fno >= 0);
		
		if ( TDno )
			fld = &ttab[TDno].TTfields[fno];	// Field in a table
		else
			fld = getvars(fno);					// normal variable
          
		if ( fld->FLDtype == 'D' )
			xtab->width = 0x0400;           // bit mask to signify special behaviour
		else
			xtab->width = getdwid(fld);		// default width
          
		if ( symbol != 930 )
			symbol = getsym();
		return xtab;
    }
	else if ( symbol == 2200 )		// numeric literal follows
    {
		xtab->width = strtol(sym, 0, 10);
        symbol = getsym();
        return xtab;
	}
    else if ( symbol == 2230 )		// variable/field name follows
	{
		ExpNo = loadexp(0, 1);
		if ( ExpNo )
		{
			xtab->WidthEXP = ExpNo;
			return xtab;
		}
	}
	else
		loaderr(37, sym);			// "invalid field width"

	return 0;
}

#endif
