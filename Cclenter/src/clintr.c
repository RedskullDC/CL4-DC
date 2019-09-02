#ifndef CLINTR_C
#define CLINTR_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include <termio.h>		// for ioctl etc.
#include <unistd.h>		// for isatty(fileno(stdout))
#include "DBdefs.h"
#include "cl4.h"

void cl_intr(int a1)
{
    if ( a1 )
    {
        signal(SIGINT, SIG_IGN);
        setvar(v_INTERRUPT, 1);		// "INTERRUPT" 17
    }
    else
        signal(SIGINT, cl_intr);
}

#endif

