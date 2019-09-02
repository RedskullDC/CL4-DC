#ifndef FLAGCBD_C
#define FLAGCBD_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"

void flagcdb(int a1, int NewPID)
{
    register int DBno;
	
    for ( DBno = 0; DBno < no_dtabs; DBno++ )
    {
        if ( *dtab[DBno].FullDBname && dtab[DBno].DBno >= 0 )
			clfork(dtab[DBno].DBno, a1, NewPID);
    }
}


#endif

