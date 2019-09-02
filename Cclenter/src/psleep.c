#ifndef PSLEEP_C
#define PSLEEP_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"

void psleep(ENTAB *entab)
{
    char s[32];
	
    s[0] = 0;
    if ( entab )
    {
        exptobuf(s, entab, 0);
		sleep(strtol(s, 0, 10));	// value in seconds
    }
}

#endif

