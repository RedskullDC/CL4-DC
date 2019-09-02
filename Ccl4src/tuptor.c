#ifndef TUPTOREC_C
#define TUPTOREC_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

void tuptor(char *dest, POS *Pos, char *src, TDef *tdef)
{
	short v4;
	short cbridx;

	//printf("tuptor(dest = x%08X, Pos = x%08X, src = x%08X, TDptr= x%08X)\n", dest,Pos,src,tdef);

	while ( tdef->TDFentry)
	{
		if ( Pos )	// POS tables allow fields to be re-positioned in target data area.
		{
			cbridx = cdbrindex(Pos, tdef->TDFentry);
			if ( cbridx >= 0 )		// Possible to not have a POS record for each field!
			{
				v4 = domtox(&dest[cbridx], src, tdef->TDFsize, tdef->TDFtype);
			}
			else	// We don't want this field! Bump to next one
			{
				if ( tdef->TDFtype & tdf_STRING ) // string variable
					v4 = _fwidth(src, tdef->TDFsize, tdef->TDFtype);
				else
					v4 = tdef->TDFsize;
			}
			src += v4;
		}
		else	// No Pos record, just save record to Memory in same order as on disk
		{
			if ( tdef->TDFtype & tdf_STRING ) // string variable
				v4 = _fwidth(src, tdef->TDFsize, tdef->TDFtype);
			else
				v4 = tdef->TDFsize;
			
			src += cpybuf(dest, src, v4);	// dest buffer is unpacked!
			dest += tdef->TDFsize;
		}
		
		tdef++; 
	}
}

#endif
