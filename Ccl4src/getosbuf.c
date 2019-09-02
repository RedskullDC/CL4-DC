#ifndef GETOSBUF_C
#define GETOSBUF_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <termio.h>		// for ioctl etc.
#include <unistd.h>		// for isatty(fileno(stdout))
#include "DBdefs.h"
#include "cl4.h"


// return current offset into Attribute/Character buffers
short getosbuf(void)
{
    short ScreenOffset;

    ScreenOffset = _cx + _co * (_cy - 1) - 1;
    if ( ScreenOffset < 0 || _co * _li <= ScreenOffset )	// quick range check
        ScreenOffset = 0;
    return ScreenOffset;
}

#endif
