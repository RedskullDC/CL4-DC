#ifndef SCANBUFF_C
#define SCANBUFF_C

#include "DBdefs.h"
#include "cl4.h"

short scnbuf(char *a1, short TDFsize, char Value)
{
	char *i;
	for ( i = a1; TDFsize; --TDFsize )
	{
		if ( *i == Value )
			break;
		i++;
	}
	return (short)(i - a1);
}

#endif
