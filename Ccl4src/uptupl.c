#ifndef UPDTUPL_C
#define UPDTUPL_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

// not public in libcl4

bool inlist_0(short *v2, short a2)
{
	//printf("inlist_0(List: x%08X, a2: %d)\n",v2,a2);

	while( *v2 )
	{
		if (*v2++ == a2)
			return true;
	}
	return false;
}

// only called from _update() 
int _uptuple(char *Dest, char *Buffer, char *a3, POS *Pos, TDef *tdef, short *a6)
{
	char *currptr;
	short v11;
	size_t v14;
	size_t NumBytes; 

	//printf("_uptuple(Dest: x%08X,Buffer: x%08X,a3: x%08X,POS: x%08X, tdef: x%08X, a6: x%08X)\n" ,Dest,Buffer,a3,Pos,tdef,a6);

	for ( currptr = Dest; tdef->TDFentry; tdef++ )
	{
		if ( tdef->TDFtype & tdf_STRING )	// string variable
			NumBytes = _fwidth(Buffer, tdef->TDFsize, tdef->TDFtype);
		else
			NumBytes = tdef->TDFsize;
    
		if ( Pos )
		{
			v11 = cdbrindex(Pos, tdef->TDFentry);
			if ( v11 >= 0 )
				currptr += xtodom(currptr, tdef->TDFsize, tdef->TDFtype, &a3[v11]);
			else
				currptr += cpybuf(currptr, Buffer, NumBytes);
		}
		else
		{
			if ( !a6 || inlist_0(a6, tdef->TDFentry) )	// a6 points to a null terminated array of TDFentry values
			{
				if ( tdef->TDFtype & tdf_STRING )	// string variable
					v14 = _fwidth(a3, tdef->TDFsize, tdef->TDFtype);
				else
					v14 = tdef->TDFsize;
				currptr += cpybuf(currptr, a3, v14);
			}
			else
			{
				currptr += cpybuf(currptr, Buffer, NumBytes);
			}
			a3 += tdef->TDFsize;
		}
		Buffer += NumBytes;
	}
	return currptr - Dest;		// bytes copied
}

#endif
