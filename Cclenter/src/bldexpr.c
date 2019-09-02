#ifndef BUILDEXPR_C
#define BUILDEXPR_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

EXPR* bldexpr(int TTno, int FieldNo, int SrchMode)
{
	EXPR	*v12;
	EXPR	*v17;
	FLDdesc *Fld;
	char	*Src;
	
	double	v21;
	int		v19;
	int		CurrFldNo;
	int		v22;
	size_t	NumBytes;

	short	Operator;
	short	v24;
	char	WildChar;
	char	last;
	char	ExprData[142];

	//printf("bldexpr(TDno: %d,FieldNo = %d, SrchMode = %C)\n" ,TTno,FieldNo,SrchMode);

	NumBytes = 0;
	Src = 0;
	if ( FieldNo < 0 )			// delset() passes -1 in here
		FieldNo = 0x200u;		// special flag, see below includes *all* key fields
	
	WildChar  = 0;
	v17		  = 0;
	CurrFldNo = 0;
	Fld		  = ttab[TTno].TTfields;
	
	while ( Fld->FLDstat & fld_KEY && FieldNo >= CurrFldNo )	// Can only build expression using Key fields
	{
		v19 = 0;
		if ( CurrFldNo == FieldNo )
		{
			if ( Fld->FLDtype == 'C' )
				WildChar = SrchMode;
			else if ( SrchMode )	// Numeric type
				WildChar = '+';		// Only allow >= searches for numeric types ??  *CL4 ERROR?!?*
		}
		if ( !(Fld->FLDstat & fld_DATA_AVAIL) )	// field has *no* data loaded at *fld->FLDdata
			rectott(TTno, CurrFldNo);

		if ( Fld->FLDtype == 'C' )
		{
			if ( !(Fld->FLDstat & fld_DATA_AVAIL))	// 0x10) )
				*(char*)Fld->FLDdata = 0;			// char pointer. set single byte to zero

			NumBytes = lenstr(Fld->FLDdata);
			if ( NumBytes == 1 )
				v19 = WildChar ? 1 : 0 ;
		}
		else
		{
			v21 = Fld->FLDstat & fld_DATA_AVAIL ? *(double*)Fld->FLDdata : 0.0;
			if ( WildChar && v21 == 0.0 )
				v19 = 1;
			else
				NumBytes = Fld->FLDlen;
		}

		if ( !v19 )		// v19 can only be true id SrchMode != 0
		{
			switch ( Fld->FLDtype )
			{
				case 'B':
				case 'I':
					v24 = (short)v21;
					Src = (char*)&v24;
					break;
				case 'D':
				case 'N':
					v22 = (int)v21;
					Src = (char*)&v22;
					break;
				case 'C':
					Src = Fld->FLDdata;
					NumBytes = lenstr(Fld->FLDdata);
					if ( NumBytes && WildChar )
					{
						last = Src[NumBytes - 1];
						if (last == '+' || last == '*' || last == '-') // look for, and 
							Src[--NumBytes] = 0;		// remove wildcard char from buffer
					}
					else if ( !WildChar )
					{
						if ( Fld->FLDlen <= NumBytes )
							NumBytes = Fld->FLDlen;
						else
							NumBytes++;	// allow room for \0 terminator?
					}
					break;
				default:
					Src = (char*)&v21;	// double
					break;
			}
		}
		if ( !v19 || FieldNo == 0x200 )	// if v19 was true, Src is uninitialised at this point. *CRASH*
		{
			xtodom(ExprData, NumBytes, Fld->TDFtype, Src);
			switch (WildChar)
			{
				case '+' :
					Operator = 0x30u;	// 0x30 == greater than or equal	'>='
					break;
				case '-' :
					Operator = 0x18u;	// 0x18 == less than or equal		'<='
					break;
				default:	// 0x00
					Operator = 0x10u;	// 0x10 == equal to					'='
					break;
			}
			v12 = newexpr(Fld->FLDelemID, Operator, ExprData, NumBytes);	// don't simplify. GCC opt error! 
			if ( v17 )
				v12 = newjunc(v17, 2, v12);	// 2 == AND of two expressions
			v17 = v12;
		}
		CurrFldNo++;
		Fld++;
	}
	return v17;
}

#endif
