#ifndef TDTOTD_C
#define TDTOTD_C

#include <sys/param.h>		// for MIN/MAX
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"		// for bit field values

//void DumpBlock(char* Buffer, int NumBytes);

bool tdtotd(int TDdest, int TDsrc, char KeyOnly, int CopyMode)
{
	TDesc	*TTptr_dest;
	TDesc	*TTptr_src;
	
	FLDdesc *fld_src;
	FLDdesc *fld_dest;
	FLDdesc *v13;
	
	char	*Src;
	char	*Dest;
	
	short	FieldNo;
	int		recsize;
	
	bool	DataCopied	= false;
	bool	FLDmatch	= false;

	//printf("tdtotd(TDdest = %d,SrcTD = %d,a3 = %d, CopyMode = %d)\n" ,TDdest,TDsrc, a3, CopyMode);

	TTptr_src	= &ttab[TDsrc];
	TTptr_dest	= &ttab[TDdest];
	
	rtdget(TTptr_src);			// appears pointless code, but will throw exceptions
	rtdget(TTptr_dest);
	
	Src			= TTptr_src->TDworkarea1;
	Dest		= TTptr_dest->TDworkarea1;
	fld_src		= TTptr_src->TTfields;
	fld_dest	= TTptr_dest->TTfields;

	for ( FieldNo = 0; fld_dest->FLDelemID; fld_dest++ )
	{
		if ( fld_dest->FLDstat >= 0 && (KeyOnly & 1))				// Only copy data for key Fields?
			break;
		
		if ( fld_dest->FLDlen )
		{
			v13 = fld_src;
			do
			{
				if ( fld_dest->FLDelemID == fld_src->FLDelemID )	// ok, we found a field match between src snd dest TDs
				{
					if ( CopyMode == 1 )	// Copy data from Src->WorkArea1 -> Dest->WorkArea1
					{
						if ( fld_src->FLDstat & fld_REC_LOADED )	// Data is available in src->WorkArea1?
						{
							DataCopied = true;
							cpybuf(Dest, Src, MIN(fld_dest->FLDlen,fld_src->FLDlen));	// ensure no dest buffer overrun
							fld_dest->FLDstat |= fld_REC_LOADED;
						}
						else
							fld_dest->FLDstat &= ~fld_REC_LOADED;	// indicate no data avail in dest for this field
					}
					else	// Copy data from fld_src->FLDdata -> fld_dest->FLDdata for matching fields
					{
						if (!(fld_src->FLDstat & fld_DATA_AVAIL))	// no data in mem?
							rectott(TDsrc, FieldNo);				// go load it to rec area
						
						if ( fld_dest->FLDtype == 'C' )
							recsize = fld_dest->FLDlen + 1;
						else
							recsize = sizeof(double);
						cpybuf((char *)fld_dest->FLDdata, (char *)fld_src->FLDdata, recsize);
						
						fld_dest->FLDstat |= 0x0018;				// set data loaded 0x10 and ??? 0x08 flags
						DataCopied = true;
						
						if ( fld_src->FLDstat & fld_ZERO )			// copy quick zero flag from src to dest
							fld_dest->FLDstat |= fld_ZERO;
						else
							fld_dest->FLDstat &= ~fld_ZERO;
					}
					FLDmatch = true;
				}
				Src += fld_src->FLDlen;
				fld_src++;
				FieldNo++;
				// reached end of source TDefs, reset for another run through
				// Dest fields may not be in same order as Src, so go around until fld_dest->FLDelemID == 0000
				if ( !fld_src->FLDelemID )	
				{
					Src		= TTptr_src->TDworkarea1;
					fld_src = ttab[TDsrc].TTfields;
					FieldNo = 0;	// reset for next loop through
				}
			}
			while ( !FLDmatch && fld_src != v13 );
			FLDmatch = false;
		}
		Dest += fld_dest->FLDlen;
	}
	return DataCopied;	// return true if data was transferred from Src -> Dest
}

#endif
