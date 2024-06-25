#ifndef DIFFKEY_C
#define DIFFKEY_C

#include <stdio.h>
#include <stdarg.h>				// for var args stuff
#include <sys/param.h>          // for MIN/MAX

#include "DBdefs.h"
#include "cl4.h"

// ********    Potential name conflict with diffkey()!!!  *********

int _diffkey(char *a1, char *a2, TDef *tdef)
{
	short	TDFtype;
	short	TDFsize;
	unsigned int v7;
	unsigned int v13;
	int		v14;
	short	v9;
	short	v10;
	
	//printf("_diffkey(a1 = %s, a2 = %s, TDef = x%08X)\n",a1,a2,tdef);

	v14 = 0;
	while ( tdef->TDFentry && (TDFtype = tdef->TDFtype, TDFtype & tdf_KEY)) // indicates Key field
	{
		TDFsize = tdef->TDFsize;

		if ( TDFtype & tdf_STRING )		// string type, variable length
		{
			v13 = _fwidth(&a1[v14], TDFsize, TDFtype);
			v7	= _fwidth(&a2[v14], TDFsize, TDFtype);
		}
		else
		{
			v13 = TDFsize;		// both same size for numeric fields
			v7	= TDFsize;
		}

		// if fields differ in size, only compare minimum:  MIN(v7,v13)
		v9 = _diffattrib(&a1[v14], &a2[v14], MIN(v7,v13), TDFtype);	// v9 = numbytes that match
		//printf("_diffkey Match Bytes = %d\n",v9);
		
		if ( v9 < v13 || v9 < v7 )	// bytes that matched were less than either field size!
		{
			v14 += v9;
			if ( TDFtype & 0x10 && (v10 = numsz(&a2[v14], v7 - v9)) != 0 )
				v14 += v10;
			else
				v14++;	// min 1 byte for the \0
			//printf("_diffkey(v9<v7/13 v14 = %d)\n",v14);
			return v14;
		}
		v14 += v9;
		tdef++;
	}
	return v14;
}
#endif
