#ifndef ROWSIZ_C
#define ROWSIZ_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

int _rowsize(TDef *tdef)		// *maximum* size in bytes required to store a "row"
{
	int i; 

	for ( i = 0; tdef->TDFentry; tdef++ )
		i += tdef->TDFsize;
	return i;
}

int _rowmin(TDef *tdef)		// *minimum* size in bytes required to store a "row"
{
	int v2;

	for ( v2 = 0; tdef->TDFentry ; tdef++ )
	{
		if ( tdef->TDFtype & tdf_STRING )
			v2++;	// string field = 1 char for \0 byte if string blank
		else
			v2 += tdef->TDFsize;
	}
	return v2;
}

#endif
