#ifndef ARGENV_C
#define ARGENV_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

extern char **environ;

char *clgetenv(char *EnvVarName)			// not called in clenter
{
    envvect = environ;						// ** global ** ENV array
    return dosrch(EnvVarName, environ);
}

char *dosrch(char *Key, char **ENVarr)
{
	char	*v4;
	int		KeyLen;
	
	//printf("dosrch('%s','%s')\n",Key,*ENVarr);
    KeyLen = strlen(Key);
    while ( *ENVarr )
    {
		if ( cmpbuf(Key, *ENVarr, KeyLen) )
		{										// ok, we found a match here
			v4 = *ENVarr;
			if ( v4[KeyLen] == '=' )			// ensure it is a key=value def.
			{
				v4 = &v4[KeyLen + 1];			// position after the '=' sign
				if ( strlen(v4) > 1000 )		// 1000 == max string size in CL4
					v4[1000] = 0;

				return v4;
			}
		}
		ENVarr++;
    }
	return 0;	// argarray null or no match
}

char *getarg(char *Key)
{
    return dosrch(Key, argvect);		// argvect is set to command line argv[]
}

#endif
