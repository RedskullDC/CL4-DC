#ifndef NEGATE_C
#define NEGATE_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

void negate(unsigned char *a1, short TDFsize)
{
	unsigned char *i; // edx@1
	char v3; // ch@4

//printf("Negate\n");
//	DumpBlock(a1,TDFsize);
	for ( i = a1; i < &a1[TDFsize]; ++i )
		*i = ~*i;
//	DumpBlock(a1,TDFsize);
	if ( a1 < i )
	{
		do
		{
			--i;
			v3 = (unsigned short)((*i)++ + 1) >> 8;
//			printf("Negate v3 = x%02X \n", v3);
		}
		while ( a1 < i && v3 & 1 );
	}
//	DumpBlock(a1,TDFsize);

}

#endif
