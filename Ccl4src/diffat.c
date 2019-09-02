#ifndef DIFFAT_C
#define DIFFAT_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

// only called by _diffkey()
int _diffattrib(char *a1, char *a2, unsigned int MaxLen, short TDFtype)
{
	char *v4;
	int i;
	unsigned char v6; // eax@4
	unsigned char v7; // esi@4

    //printf("_diffattrib(a1: x%08X, a2: x%08X, MaxLen = %d, TDFtype: x%04X)\n" ,a1,a2,MaxLen,TDFtype);

	if ( TDFtype & 0x4000 )	// ???
	{
		v4 = a1;	// swap src and dest ??   Doesn't seem to be any point??
		a1 = a2;
		a2 = v4;
	}

	for ( i = 0; i < MaxLen; i++ )
	{
		v6 = a1[i];
		v7 = a2[i];
		if ( v6 != v7 )
		{
			if ( !(TDFtype & tdf_NOCASE) )		// TDFtype & 0x08 allows non-case sensitive match
				break;

			if ((v6 - 65) <= 0x19u)			// check for a match between upper/lower case
				v6 += 32;					// tolower()
			
			if ((v7 - 65) <= 0x19u)
				v7 += 32;					// tolower()

			if (v6 != v7)
				break;
		}
	}
	return i;	// return number of chars that matched. same as MaxLen if equal
}

#endif



