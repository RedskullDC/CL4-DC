#ifndef FLDXFER_C
#define FLDXFER_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

void fldxfer(int TDdest, char *DestBuff, int TDsrc, char *Src)
{
    char	*SrcBuff;
	TDef	*tdefSrc;
	TDef	*tdefDest;
    short	v7;
	short	DestBytesLeft;
	short	SrcBytesLeft;
	bool	isNeg;
	
	//printf("fldxfer TDdest = %3d, TDsrc = %3d\n",TDdest,TDsrc); 
    tdefDest = tblrow(TDdest);
    if ( tdefDest && tblrow(TDsrc))
    {
		while ( tdefDest->TDFentry )	// go through all dest tdefs
		{
			SrcBuff = Src;
			tdefSrc = tblrow(TDsrc);

			// iterate through Src Tdef's looking for end of list or a match 
			while ( tdefSrc->TDFentry && tdefSrc->TDFentry != tdefDest->TDFentry )
			{
				SrcBuff += tdefSrc->TDFsize;
				tdefSrc++;
			}

	        if ( tdefSrc->TDFentry == tdefDest->TDFentry )	// found a match, copy data
			{
				v7 = tdefSrc->TDFtype;
                if ( v7 & 0xFC0 )							// numeric type
                {
					if ( v7 & tdf_SIGNED && *SrcBuff < 0 )		// 0x04 == allow signed value. check for negative number - high bit set
					{
						isNeg = true;
                        negate(SrcBuff, tdefSrc->TDFsize);	// negate src buffer prior to xfer
					}
					else
						isNeg = false;
                        
					SrcBytesLeft	= tdefSrc->TDFsize;
					DestBytesLeft	= tdefDest->TDFsize;

					while ( DestBytesLeft > 0 && SrcBytesLeft > 0)	// copy Src/Dest num bytes, whichever is least
						DestBuff[--DestBytesLeft] = SrcBuff[--SrcBytesLeft];

					while ( DestBytesLeft > 0 )				// dest is bigger than src, pad start with \0's
						DestBuff[--DestBytesLeft] = 0;

					if ( isNeg )
                    {
						negate(SrcBuff, tdefSrc->TDFsize);	// negate src buffer back.
                        if ( tdefDest->TDFtype &tdf_SIGNED )
							negate(DestBuff, tdefDest->TDFsize);
					}
				}
				else		// string type, just copy the buffer. MaxLen = Dest Buffer size
				{
					cpybuf(DestBuff, SrcBuff, tdefDest->TDFsize);
				}
			}
			// Possible that no match was found. If so, dest buffer area for that field is not disturbed
			DestBuff += tdefDest->TDFsize;
			tdefDest++;
		}
		return;	// exit_success
    }
}

#endif
