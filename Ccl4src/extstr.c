#ifndef EXTRACT_STR_C
#define EXTRACT_STR_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

char buf_0_1[8192];

char *extstr(char *src, int Seperator, char **a3)
{
	char *v8;
	char *v12;
    char v4;
	
    buf_0_1[0] = 0;

    if ( !src )
        goto LABEL_29;

    if ( Seperator == 32 )	// space
    {
        while ( *src && isspace(*src))		// isspace()  ==	0x2000
            ++src;

        if ( ((v4 = *src, v4 == '\'') || v4 == '"') && (v12 = strchr(src + 1, v4)) != 0 )		// look for quoted string
        {
            v12++;
            strncpy(buf_0_1, src, v12 - src);
            buf_0_1[v12 - src] = 0;
            src = v12;
        }
        else	// not a quoted string
        {
            v8 = buf_0_1;
            while ( *src && !isspace(*src) )		// isspace()  ==	0x2000
                *v8++ = *src++;
            *v8 = 0;
        }
    }
    else
    {
        v8 = buf_0_1;
        while ( *src && *src != Seperator )
			*v8++ = *src++;
        *v8 = 0;
        if ( *src )
            ++src;
    }

    if ( src && *src )	// more data left in the string after the sep char.
        *a3 = src;
    else
LABEL_29:
        *a3 = 0;

    return buf_0_1;
}

#endif
