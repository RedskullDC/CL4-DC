#ifndef COORD_C
#define COORD_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values


// doesn't make a great deal of sense.
// If _cx or _cy are outside the bounds of the current screen, treat them as if they had wrapped.

// Internally, CL4 allows 512x512 screen area.
// Whereat() already performs bounds checking 

inline void coord(void)
{
    for ( ; _co < _cx; ++_cy )			// bump cy when _cx wraps
        _cx -= _co;
    
	for ( ; _li < _cy; _cy -= _li )		// just wrap _cy by current screen length
        ;
}

#endif
