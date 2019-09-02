#ifndef XTODOM_C
#define XTODOM_C

#include <stdio.h>
#include <stdarg.h>			// for var args stuff
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"		// for bit field values

int xtodom(char *Dest, short FLDlen, unsigned short TDFtype, char *Src)
{
	int		v4;
	int		v8;
	unsigned short v6;
	short	v7;
	
	double	v10;
	double	v5	= 0.0;
	int		v12	= 0;
	short	v13	= 0;
	
	bool	Negative;

	//printf("xtodom() - FLDlen = %d, TDFtype = x%04X \n", FLDlen, TDFtype);
	//DumpBlock(Src,FLDlen);

	v4 = FLDlen;
	if ( TDFtype & 0x1C0 )											// 0000 0001 1100 0000	Numeric type
		v6 = (TDFtype & 0x1C0u) >> 6;
	else
	{
		v6 = (TDFtype & 0x0E00) ? ((TDFtype & 0x0E00) >> 9) : 0 ;	// 0000 1110 0000 0000	Numeric type
	}

//-----------------------

	v7 = v6 - 1;
	v8 = (v6 - 1);
	if ( v7 >= 0 )	// Ture if we are looking a numeric variable type
	{
		
		Negative = ((TDFtype & tdf_SIGNED) != 0);	//	0x0004 indicates allow signed value

		if ( v8 <= 0 && FLDlen <= 4 )	// integer types
		{
			if ( FLDlen <= 2 )	// FLDlen = 1 or 2
			{
				v13 = *(short*)Src;
				if ( v13 >= 0 )
					Negative = false;
				if (Negative)
					v13 = -v13;			// abs value. for rounding purposes. reverse later if necessary
			}
			else	// FLDlen = 3 or 4
			{
				v12 = *(int*)Src;
				if ( v12 >= 0 )
					Negative = false;
				if ( Negative )
					v12 = -v12;
			}
		}
		else	// v8 > 0 || FLDlen > 4 
		{
			v5 = *(double*)Src;
			if ( v5 >= 0.0 )
				Negative = false;
			else					// negative numbers always allowed for doubles
				v5 = -v5;
		}

		if ( v8 > 0 )
		{
			if ( TDFtype & 0x1C0 )
				v10 = v5 * _dbias[v8];
			else
				v10 = _addexp(v5, 8 * v8);
			v5 = v10 + 0.5;

			/*if ( FLDlen > 2 )	// no need to test these values? Just set V12, 13 anyway?  May or may not be used below
			{
				if ( FLDlen <= 4 )
					v12 = (int)v5;
			}
			else
				v13 = (short)v5;
			*/
			v12 = (int)v5;
			v13 = (short)v5;
		}
    
		switch ( FLDlen )
		{
			case 1:
				*Dest = (char)v13;					// byte || char
				break;
			case 2:
				itoms((short*)Dest, v13);			// short
				break;
			case 3:
				xitoms((int*)Dest, v12);			// ????
				break;
			case 4:
				ltoms((int*)Dest, v12);				// long
				break;
			case 6:
				xltoms((long long*)Dest, v5);		// long long
				break;
			default:								// double (length == 8)
				if ( FLDlen > 7 )
					dtoms((double*)Dest, v5);
				break;
		}
		if ( Negative )
			negate(Dest, FLDlen);
		//DumpBlock(Dest,FLDlen);
	}
	else
	{
		if ( TDFtype & tdf_STRING )					// 0x0002 = string value
			v4 = cpyseq(Dest, Src, FLDlen);
		else
			cpybuf(Dest, Src, FLDlen);
	}
	return v4;
}

#endif
