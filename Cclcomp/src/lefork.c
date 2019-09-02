#ifndef LEFORK_C
#define LEFORK_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

int lefork(PTAB *pt)
{
    int		TDno;
	short	ENTno;
    char	FieldType;

    symbol = getsym();
    if ( symbol == 930 )						// fork with no arguments is ok
		return 1;
    
	if ( symbol == 2230 || symbol == 2220 )		// string literal or string variable
	{
		ENTno = loadexp(0, 0);					// also advances to next symbol
		if ( ENTno )
		{
			gettf(ENTno, &TDno, &FieldType);
			if ( FieldType == 'C' )
			{
				pt->TABno = ENTno;
				if ( symbol == 930 )			// <CR> should follow
					return 1;					// exit_success
				else
					loaderr(33, sym);			// unexpected symbol
			}
			else
				loaderr(64, sym);				// "'fork' command must be character"
		}
    }
	else
		loaderr(64, sym);	// "'fork' command must be character"

	return 0;	// error exit
}

#endif
