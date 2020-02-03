#ifndef RECTOTT_C
#define RECTOTT_C

#include "DBdefs.h"
#include "cl4.h"

void rectott(int TTno, int FieldNo)
{
	TDesc	*TTptr;
	FLDdesc *fld;
	char	*Workarea1;
	double	tmpDouble;
	int		tmpInt;
	short	tmpShort;
	char	FLDtype;

	//printf("rectott(TTno = %2d, FieldNo = %2d) - " ,TTno,FieldNo);

	TTptr	= &ttab[TTno];
	fld		= TTptr->TTfields;

	Workarea1 = TTptr->TDworkarea1;
	while ( FieldNo )				// Fields are zero based internally, 1 based externally
	{
		Workarea1 += fld->FLDlen;	// Advance Workarea1 and fld to the required FieldNo
		++fld;
		--FieldNo;
	}
//-------------
	if ( fld->FLDlen )
	{
		FLDtype = fld->FLDtype;
		if ( fld->FLDstat & fld_REC_LOADED)	// 0x0001 data in TTptr->Workarea for this field
		{
			switch ( FLDtype )
			{
				case 'B':
				case 'I':
					domtox((char*)&tmpShort, Workarea1, fld->FLDlen, fld->TDFtype);
					tmpDouble = (double)tmpShort;		// Keep all variables as double internally
					break;
				case 'D':
				case 'N':
					domtox((char*)&tmpInt, Workarea1, fld->FLDlen, fld->TDFtype);
					tmpDouble = (double)tmpInt;
					break;
				case 'C':
					domtox(fld->FLDdata, Workarea1, fld->FLDlen , fld->TDFtype);
					if ( *(char *)fld->FLDdata )			// String != null
					{
						fld->FLDstat &= ~fld_ZERO;			// 0xFFFB;  clear ZERO flag (bit 2)
						fld->FLDstat |= fld_DATA_AVAIL;		// 0x0010u; set DATA_AVAIL flag
					}
					else
						fld->FLDstat |= (fld_DATA_AVAIL|fld_ZERO);	// 0x0014u; set ZERO & DATA_AVAIL flags
					return;
					break;
				default:
					//printf("rectott: def Calling domtox(FLDlen: %d, FLDtype: x%04X\n",fld->FLDlen, fld->TDFtype);  
					domtox((char*)&tmpDouble, Workarea1, fld->FLDlen , fld->TDFtype);
					if ( FLDtype == 'G' || FLDtype == 'R' || FLDtype == 'T' )
						tmpDouble = clround(tmpDouble, FLDtype);
					break;
			}
			//----------------
			*(double *)fld->FLDdata = tmpDouble;
			if ( tmpDouble == 0.0 )
				fld->FLDstat |= (fld_DATA_AVAIL|fld_ZERO);	// 0x0014u; set ZERO & DATA_AVAIL flags
			else
			{
				fld->FLDstat &= ~fld_ZERO;			// 0xFFFB;  clear ZERO flag (bit 2)
				fld->FLDstat |= fld_DATA_AVAIL;		// 0x0010u; set DATA_AVAIL flag
			}
		}
		else	// *NO* data in TTptr->Workarea for this field, just zap the fld data area
		{
			if ( FLDtype == 'C' )
				memset(fld->FLDdata, 0,fld->FLDlen);
			else
				*(double *)fld->FLDdata = 0.0;
				//tmpDouble = 0.0;	// *** doesn't look right *** Isn't updating workarea!
			fld->FLDstat |= (fld_DATA_AVAIL|fld_ZERO);	// 0x0014u; set ZERO & DATA_AVAIL flags
		}
	}
	return;
}

#endif
