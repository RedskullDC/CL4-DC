#ifndef FWIDTH_C
#define FWIDTH_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

short _fwidth(char *a1, short TDFsize, short TDFtype)
{
	short BufSz; 
	//printf("_fwidth(%08X, %2d, %02X) ",a1, TDFsize, TDFtype );

	if ( TDFtype & tdf_STRING && (BufSz = scnbuf(a1, TDFsize, 0), BufSz < TDFsize))	// if char* , check actual length
		return (BufSz + 1);	// allow for \0 terminator byte with string
	else
		return TDFsize;		// Numeric = fixed width
}

#endif
