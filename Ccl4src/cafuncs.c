#ifndef CA_FUNCS_C
#define CA_FUNCS_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

char **camake(char *a1, char Seperator)
{
	char **v3;
	char **v6;
	char *a3;
	char *v4;
	
	int Pieces;

	Pieces = 0;									// calculate how many "pieces" in the string
	a3 = a1;
	while ( *extstr(a3, Seperator, &a3) )
		Pieces++;

	v6 = (char **)mmalloc(4 * (Pieces + 1));	// space for array of char pointers to the string pieces, null terminated
	v3 = v6;
	a3 = a1;
	while ( 1 )
	{
		v4 = extstr(a3, Seperator, &a3);
		if ( !*v4 )
			break;
		*v3 = mstrcpy(v4, 0);					// make copies of all "pieces"
		v3++;
	}
	*v3 = 0;
	return v6;
}

int cafree(char **ptr)
{
    char **v1;
	char *v2;
	
    v1 = ptr;
    if ( ptr )
    {
        if ( *ptr )
        {
            do
            {
                v2 = *v1;
                v1++;
                mfree_0(v2);
            }
            while ( *v1 );
        }
        mfree_0(ptr);
    }
    return 0;
}

#endif
