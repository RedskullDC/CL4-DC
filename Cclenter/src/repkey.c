#ifndef REPKEY_C
#define REPKEY_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

// cmprec *not* public in libcl4
signed int cmprec(int TTno, int TTno2)
{
	FLDdesc		*fld;
	FLDdesc		*fld2;
	TDesc		*TTptr;
	FLDdesc		*v7;
	size_t		cmp_length;
	int			result;
	short		FieldNo;
	bool		FLDmatch;

	//printf("cmprec(TTno = %d,TTno2 =  %d)\n" ,TTno, TTno2);

	FLDmatch = false;
	FieldNo  = 0;
	TTptr	 = &ttab[TTno2];
	fld		 = TTptr->TTfields;
	fld2	 = ttab[TTno].TTfields;
	if ( fld2->FLDelemID )
	{
		while ( !fld2->FLDlen )
		{
LABEL_19:
			fld2++;
			FieldNo++;
			if ( !fld2->FLDelemID )
				return 0;
		}
//-----------------------------------
		v7 = fld;
		while ( 1 )
		{
			if ( fld2->FLDelemID == fld->FLDelemID )
			{
				if ( !(fld2->FLDstat & fld_DATA_AVAIL))	// 0x10) )
					rectott(TTno, FieldNo);
				
				cmp_length = fld->FLDtype == 'C' ? fld->FLDlen : sizeof(double);
				FLDmatch = true;
				if ( !cmpbuf((char *)fld->FLDdata, (char *)fld2->FLDdata, cmp_length) )	// Returns 1 if match all bytes == TRUE
					break;					// found a field that *didn't* match - EXIT
			}
			++fld;
			if ( !fld->FLDelemID )
				fld = TTptr->TTfields;

			if ( FLDmatch || fld == v7 )	// found a match, or looked at all fields of TTno. go around again
			{
				FLDmatch = false;
				goto LABEL_19;
			}
		}
//------------------------------------
		if ( fld2->FLDstat >= 0 )			// we are past the key area
			result = 2;						// one of the normal data fields didn't match
		else
			result = 1;						// one of the Key Fields didn't match?
	}
	else
	{
		result = 0;
	}
	return result;
}


void repkey(int TDno, int NameSrch)
{
	TDesc	*TTptr2; 
	TDesc	*TTptr; 
	FLDdesc *fld; 

	char	*v7;
	
	int		FieldNo;
	int		TDno2;
	int		v10;
	int		v14;
	int		ErrorCode;
	short	v3;
	
	//printf("repkey(TDno = %d, Namesrch = %d)\n" ,TDno, NameSrch);

	TTptr2 = &ttab[TDno];
	v3 = TTptr2->TDlocked;
	if ( v3 & 0x0010 )
	{
		if ( v3 & 0x0020 )
		{
			fld = TTptr2->TTfields;
			for ( FieldNo = 0; fld->FLDelemID; ++fld )
			{
				if ( fld->FLDlen && !(fld->FLDstat & fld_DATA_AVAIL))	// 0x0010) )
					rectott(TDno, FieldNo);	// make sure data is available in Fields. Copy from WorkArea if necessary
				++FieldNo;
			}
		}
//-------------------
		rdkey(TDno, NameSrch);
		for ( fld = TTptr2->TTfields; fld->FLDelemID; ++fld )
		{
			if ( fld->FLDtype == 'K' )	// Look for Key type sub-records
			{
				v7 = NameSrch ? fld->FLDname : fld->FLDdata;		// Search by Name/Data.  Not supported in libcl4
				//TDno2 = findtd(TTptr2->DBcontext, v7, TTptr2->DBnumber);	// libcl4.  No Context involved here, since our TD list is private.
				TDno2 = findtd(v7, TTptr2->DBnumber);
				if ( TDno2 >= 0 )
				{
					TTptr = &ttab[TDno2];
					if ( TTptr->TDlocked & 0x0020 )
					{
						v14 = cmprec(TDno2, TDno);
						if ( v14 == 1 )					// one of the Key Fields didn't match?
						{
							tttorec(TDno2, TTptr->TDworkarea1, 1);
							ErrorCode = cldelr(rtdget(TTptr), TTptr->TDworkarea1, 0, TTptr);
							if ( ErrorCode < 0 )
								dberror(ErrorCode, 0, rtdget(TTptr));
							TTptr->TDlocked &= 0xFFDEu;	// clear 0x0021 bits
						}
					}
					else
						v14 = 2;
					
					if ( v14 )
					{
						tdtotd(TDno2, TDno, 0, 2);
						reprec(TDno2, 0);
					}
					rfree(TDno2, 0);
				}
			}
		}
	}
}
#endif
