#ifndef DOMTOX_C
#define DOMTOX_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"		// for bit field values

short domtox(char *Dest, char *Src, short FLDlen, short TDFtype)
{
	short v4; // di@1
	unsigned short v5;	// unsigned to avoid 1 bits in shifts 
	short v6; // ax@5
	int v8; // eax@15
	int v9; // eax@20
	
	double v18 = 0.0;
	int v17 = 0;
	int v19 = 0;
	bool Negative = false;	// default to +ve number

	//printf("domtox(Dest - %s, Src %s, FLDlen %2d, TDFtype x%04X\n",Dest, Src, FLDlen, TDFtype);

	v4 = FLDlen;	// default return value

	if ( TDFtype & 0x01C0 )											// 0000 0001 1100 0000	Numeric type
		v5 = (TDFtype & 0x01C0u) >> 6;
	else
		v5 = (TDFtype & 0x0E00) ? ((TDFtype & 0x0E00) >> 9) : 0 ;	// 0000 1110 0000 0000	Numeric type
	
	v6 = v5 - 1;
	if ( v6 < 0 )													// string type
	{
		if ( TDFtype & tdf_STRING )						// 0x0002 = String. variable length field. return actual bytes copied
			v4 = cpyseq(Dest, Src, FLDlen);
		else
			cpybuf(Dest, Src, FLDlen);
		return v4;
	}

	if ( TDFtype & tdf_SIGNED && *Src < 0 )		// 0x0004 == Allow signed value bit. Check high bit of first src byte
	{
		Negative = true;
		negate(Src, FLDlen);
	}
	switch (FLDlen)
	{
		case 1 : 
			v8 = *Src;							// Byte value
			v19 = v8;
			//printf("Case:1  - v8 = %d, v19 = %d\n",v8,v19);
			//DumpBlock(Src,8);
			break;
		case 2 :								// Short value
			v8 = (unsigned char)Src[1]| (unsigned short)(*Src << 8);		
			v19 = v8;
			//printf("Case:2  - v8 = %d [x%02X], v19 = %d [x%02X]\n",v8,v8,v19,v19);
			//DumpBlock(Src,8);
			break;
		case 3 :								// ? 3 byte value
			v9 = mstoxi((int*)Src);
			v17 = v9;
			break;
		case 4:									// Int value (4bytes)
			//DumpBlock(Src, 8);				
			v9 = mstol((int*)Src);				
			//printf("domtox: v9 = %d [x%04X], Neg = %s\n",v9, v9, Negative?"true":"false");
			v17 = v9;
			break;
		case 6 : 
			v18 = mstoxl((int*)Src);
			break;
		case 8 :								// Double value
			v18 = mstod((double*)Src);
			break;
		default:
			// only reach here if an error. Other cases should catch all
			break;
	}

	if ( Negative )
		negate(Src, FLDlen);
	
	if ( v6 > 0 )
	{
		if ( FLDlen > 2 && FLDlen <= 4)
			v18 = (double)v17;
		else
			v18 = (double)v19;
		if ( TDFtype & 0x1C0 )
			v18 = v18 / _dbias[v6];
		else
			v18 = _addexp(v18, -8 * v6);
		*(double*)Dest = Negative ? -v18 : v18 ;
	}
	else if ( FLDlen > 4 )
		*(double*)Dest = Negative ? -v18 : v18 ;
	
	else if ( FLDlen <= 2 )
		*(short*)Dest = (short)(Negative ? -v19 : v19) ;
	
	else	// FLDlen == 4 
		*(int*)Dest = Negative ? -v17 : v17 ;
	
	return v4;
}

#endif
