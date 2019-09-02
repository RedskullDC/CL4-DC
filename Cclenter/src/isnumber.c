#ifndef CLISNUMBER_C
#define CLISNUMBER_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

// if not a number, returns the offending character.

char clisnumber(char *a1)
{
	char	*i;
	char	*v3;
	int		radix_chars = 0;
	char	v4;
	
	// remove blank spaces, or commas (or language specific thousands separator char)
    for ( i = a1; *i; ++i )
    {
        while ( *i == THOU_SEP[0] || *i == ' ' )
        {
            v3 = i;
            if ( *i )
            {
                do
                {
                    v4 = v3[1];
                    *v3++ = v4;
                }
                while ( v4 );
            }
        }
    }

	// See if string has a sign character
    if ( *a1 == '+' || *a1 == '-')
        a1++;							// skip past sign char

    while ( *a1 )
    {
        if ( *a1 == RADIX_CHAR[0] )		// check for '.' (or lang specific radix)
		{
            if ( ++radix_chars > 1 )	// more than 1 radix char is bad!
                return *a1;				// error_exit, return offending char
		}
        else if ( !(isdigit(*a1)) )
            return *a1;					// error_exit, return offending char
        ++a1;
    }
    return 0;	// 0 == exit_success
}

#endif
