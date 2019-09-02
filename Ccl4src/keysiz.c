#ifndef KEYSIZ_C
#define KEYSIZ_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for TDFtype bit field values

int _keysize(TDef *tdef)		// distinctly different to keysize(TDno);
{
	int KeySize;

	for ( KeySize = 0; tdef->TDFentry && (tdef->TDFtype & tdf_KEY); tdef++ )
		KeySize += tdef->TDFsize;
	return KeySize;
}

int _keymin(TDef *tdef)
{
	int KeyMin;

	for ( KeyMin = 0; tdef->TDFentry && (tdef->TDFtype & tdf_KEY); tdef++  )
	{
		if ( tdef->TDFtype & tdf_STRING )		// String field == variable length
			KeyMin++;							// Minimum size = 1 byte (for the \0 terminator) 
		else
			KeyMin += tdef->TDFsize;
	}
	return KeyMin;
}

#endif
