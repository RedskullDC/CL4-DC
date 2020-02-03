#ifndef ADDTUPLE_C
#define ADDTUPLE_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for TDFtype bit field values

// not public
void sum(char *a1, char *a2, short TDFsize)
{
	short	Carry;
	short	v5;
	short	v6;
	
	printf("sum(a1: %s,a2 = %s, TDFsize = %03d)" ,a1,a2,TDFsize);

	Carry = 0;
	v6 = TDFsize - 1;
	while ( v6 >= 0 )
	{
		v5 = a2[v6] + a1[v6];		// should these be unsigned chars?
		if ( Carry & 0x100 )		// Byte values overflowed last time around
			v5++;
		Carry = v5;
		a1[v6] = v5;
		--v6;
	}
	printf(" after (a1: %s\n",a1); 
}

void _addtuple(char *Buffer1, char *Buffer2, TDef *TDefPtr)
{
	TDef	*tdef;
	short	v5;
	short	v6;
	
	//printf("_addtuple(Buffer1: %S,Buffer2 = %Sd, TDef = x%08X)\n" ,Buffer1,Buffer2,TDefPtr);

	for ( tdef = TDefPtr; tdef->TDFentry; tdef++ )
	{
		if ( !(tdef->TDFtype & tdf_KEY) && tdef->TDFtype & 0x0FC0 )		// check these flag bits?   0000 1111 1100 0000
			sum(Buffer1, Buffer2, tdef->TDFsize);
		
		if ( tdef->TDFtype & tdf_STRING )
			v5 = _fwidth(Buffer1, tdef->TDFsize, tdef->TDFtype);
		else
			v5 = tdef->TDFsize;
		
		Buffer1 += v5;
		
		if ( tdef->TDFtype & tdf_STRING )
			v6 = _fwidth(Buffer2, tdef->TDFsize, tdef->TDFtype);
		else
			v6 = tdef->TDFsize;
		
		Buffer2 += v6;
  }
}

#endif
