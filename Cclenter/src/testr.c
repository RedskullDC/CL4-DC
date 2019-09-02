#ifndef TESTR_C
#define TESTR_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

bool testr(int TDno, char *Src, POS *pos, EXPR *expr)
{
    TDef	*tdef;
	char	*Buffer;
	bool	Match;
	
    tdef	= tblrow(TDno);
    
	Buffer	= (char *)alloc(_rowsize(tdef), 0);	// RowSized buffer
    rtotup(Buffer, Src, pos, tdef);

    Match	= _match(Buffer, expr, tdef);
    nfree(Buffer, 0);

    return Match;
}



#endif
