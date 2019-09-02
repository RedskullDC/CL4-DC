#ifndef RECTOTUP_C
#define RECTOTUP_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

short rtotup(char *dest, char *src, POS *Pos, TDef *tdef)
{
	char *buffptr;
	short v6;
	short cbridx;
	int v8;

	for ( buffptr = dest; tdef->TDFentry; tdef++ )
	{
		if ( Pos )
		{
			cbridx = cdbrindex(Pos, tdef->TDFentry);
			if ( cbridx >= 0 )
				v8 = xtodom(buffptr, tdef->TDFsize, tdef->TDFtype, &src[cbridx]);
			else
				v8 = cdbfclear(buffptr, tdef->TDFsize, tdef->TDFtype); // if no Pos entry, just zap the space
			buffptr += v8;
		}
		else // assume src buffer is laid out in TDef order
		{
			if ( tdef->TDFtype & tdf_STRING )	// string variable
				v6 = _fwidth(src, tdef->TDFsize, tdef->TDFtype);
			else
				v6 = tdef->TDFsize;
			
			buffptr += cpybuf(buffptr, src, v6); // dest buffer is (possibly) packed
			src += tdef->TDFsize;
		}
	}
	return (short) (buffptr - dest);
}

#endif
