#ifndef IS_A_LIST_C
#define IS_A_LIST_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

bool isalist(char *a1)
{
    while ( *a1 )
    {
		switch ( *a1 )
		{
			case '\n':
			case '"':
			case '$':
			case '&':
			case '\'':
			case '(':
			case '*':
			case ';':
			case '<':
			case '>':
			case '?':
			case '[':
			case '^':
			case '`':
			case '|':
				return true;
				break;
		}
		a1++;
    }
	return false;
}


#endif
