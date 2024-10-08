#ifndef GETTYPE_C
#define GETTYPE_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

/*
	Bit field values:
	0000 0000 0000 0001	: x0001 =		Key Field
	0000 0000 0000 0010	: x0002 =		variable size field
	0000 0000 0000 0100 : x0004	=		Allow signed values
	0000 0000 0000 1000 : x0008	=		Allow case-insensitive match
	0000 0000 0001 0000 : x0010	=		Allow string number comparison.

	0000 0000 0000 1010 : x000A	= 'A'	String. Nocase, Variable length. ** SYSTEM ONLY **
	0000 0000 0001 1010 : x001A	= 'C'	String. Nocase, Variable length, num compare flags all set.

	0000 0000 0100 0100 : x0044	= 'B', 'I', 'N', 'L'  INT Values (TDFsize 1,2,4,8 determines final type), allow signed
	0000 0000 1010 0100 : x00A4	= '1'	floating point types, all allow signed
	0000 0000 1000 0100 : x0084	= '2'
	0000 0000 1110 0100 : x00E4	= '3'
	0000 0000 1100 0100 : x00C4	= '4'
	0000 0001 0010 0100 : x0124	= '5'
	0000 0001 0000 0100 : x0104	= '6'
	0000 0001 0110 0100 : x0164	= '7'
	0000 0001 0100 0100 : x0144	= '8'
	0000 0001 1010 0100 : x01A4	= '9'

	0000 0100 0000 0100 : x0404	= 'G'	Value, allow signed
	0000 0110 0000 0100 : x0604	= 'R'	Value, allow signed

	0000 1000 0000 0000 : x0800	= 'T'	Time Value, unsigned
	0001 0000 0100 0000 : x1040	= 'D'	Date Value, int flag set, unsigned
	0010 0000 0000 0000 : x2000 = 'S'	Sub-Table
	0011 0000 0000 0000 : x3000 = 'K'	Key Table
	0100 0000 0000 0000 : x4000 = ???   Swaps order when comparing attributes ??? cmpattrib(), diffattrib()
*/

char gettype(unsigned short TDFtype, short TDFsize)
{
	unsigned short v3;
	char FLDtype;
	//printf("gettype(TDFtype: x%04X, TDFsize: x%04X (%4d) = " ,TDFtype,TDFsize,TDFsize);

	switch (TDFtype & 0x3000)
	{
	case 0x1000:
		FLDtype = 'D';	// 0x1040
		break;
	case 0x2000:
		FLDtype = 'S';
		break;
	case 0x3000:
		FLDtype = 'K';
		break;
	default:								//       xxx			>>9  Catches R,G,T
		v3 = (TDFtype >> 9) & 0x07;			//	---- ---- ---- ---- 
		if ( v3 || (v3 = (TDFtype >> 6) & 0x07) != 0 )		//          x xx        >>6 
		{													//	---- ---- ---- ---- 
			if ( TDFsize == 8 )
			{
				switch ( v3 )
				{
				case 1:
					FLDtype = 'L';								//	0x0044
					break;
				case 2:
					FLDtype = ((TDFtype & 0x20) == 0) + '1';	//	1 = x00A4, 2 = x0084
					break;
				case 3:
					FLDtype = ((TDFtype & 0x20) == 0) + '3';	//	3 = x00E4, 4 = x00C4
					break;
				case 4:
					FLDtype = ((TDFtype & 0x20) == 0) + '5';	//	5 = x0124, 6 = x0104
					break;
				case 5:
					FLDtype = ((TDFtype & 0x20) == 0) + '7';	//	7 = x0164, 8 = x0144
					break;
				default:
					FLDtype = '9';								//	9 = x01A4
					break;
				}
			}
			else if ( TDFsize == 4 )
			{
				switch (v3)
				{
				case 1:
					FLDtype = 'N';		//	0x0044		INT + allow signed
					break;
				case 2:
					FLDtype = 'G';		//	0x0404
					break;
				case 3:
					FLDtype = 'R';		//  0x0604
					break;
				default:
					FLDtype = 'T';		//	0x0800 only allows unsigned values
					break;
				}
			}
			else if ( TDFsize == 2 )
				FLDtype = 'I';			//	0x0044
			else
				FLDtype = 'B';			//	0x0044
		}
		else
		{
			FLDtype = 'C';
		}
		break;
	}
	//printf("%C\n", FLDtype); 
	return FLDtype;
}

#endif
