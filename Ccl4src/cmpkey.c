#ifndef CMPKEY_C
#define CMPKEY_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"                     // for bit field values

short _cmpkey(char *a1, char *a2, TDef *tdef)
{
	short TDFtype;
	int		v4	= 0;
	short	v6	= 0;

	//printf("_cmpkey(a1 = %08X, a2 = %08X, TDef = %08X)\n" ,a1, a2,tdef);

	while ( tdef->TDFentry && (TDFtype = tdef->TDFtype, TDFtype & tdf_KEY) )	// &1 = Keyfield
	{
		v6 = _cmpattrib(&a1[v4], tdef->TDFsize, &a2[v4], tdef->TDFsize, TDFtype);
		if ( v6 )
			break;

		if ( tdef->TDFtype & tdf_STRING )
			v4 += _fwidth(&a1[v4], tdef->TDFsize, tdef->TDFtype);
		else
			v4 += tdef->TDFsize;

		tdef++;	// v7 == 0 if we fall out here anyway. Set if v6 != 0 above
	}
	return v6;
}

#endif
