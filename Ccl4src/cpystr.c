#ifndef CDBCPYSTR_C
#define CDBCPYSTR_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

char *cdbcpystr(char *Dest, char *Src, ...)
{
	va_list vl;

	va_start(vl, Src);
	while ( Src )
	{
		while(*Src)
			*Dest++ = *Src++;		// No sanity check on Dest ptr.
		Src = va_arg(vl,char*);
	}	
	va_end(vl);		// keep va macros happy
	*Dest = 0;
	return Dest;	// Return end of copied string to make chaining easy :o)
}
#endif
