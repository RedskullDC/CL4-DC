#ifndef CLHINTR_C
#define CLHINTR_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include <termio.h>		// for ioctl etc.
#include <unistd.h>		// for isatty(fileno(stdout))
#include "DBdefs.h"
#include "cl4.h"

void clh_intr(int a1)
{
    int v2;

    signal(SIGTERM, SIG_IGN );
    signal(SIGHUP, SIG_IGN );
    if ( a1 )
    {
        if ( hangup == 1 )
            longjmp(sjbuf, -a1);		// crash back out to main()
        if ( hangup == 2 )
            sigSeen_0 = a1;
    }
    else if ( hangup == 1 )
	{
		signal(SIGHUP, clh_intr);
		signal(SIGTERM, clh_intr);
		if ( sigSeen_0 )
		{
			v2 = sigSeen_0;
			sigSeen_0 = 0;
			clh_intr(v2);				// recursive
		}
	}
	else if ( hangup == 2 )
	{
		signal(SIGHUP, clh_intr);
		signal(SIGTERM, clh_intr);
	}
}

#endif

