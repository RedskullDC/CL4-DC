#ifndef RECORDFREE_C
#define RECORDFREE_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

void DumpBlock(char* Buffer, int NumBytes);
void DumpTDptr(TDinfo* TD,bool ShowTDef);

/* contains rfree(), rkfree(), rdfree(), ravail(), tfree()*/

// Set field flags to indicate that data record has been loaded, and can be examined
void ravail(int TDno)
{
	TDesc	*TTptr;
	FLDdesc *fld;

	//printf("ravail(TDno = %d)\n" ,TDno);
	TTptr = &ttab[TDno];
	for ( fld = TTptr->TTfields; fld->FLDelemID; fld++ )
		fld->FLDstat |= fld_REC_LOADED;				// data in TTptr->Workarea for this field
	TTptr->TDlocked |= ttl_AVAIL;	// 0x0020u;
}

void rfree(int TDno, int FieldNo)
{
	FLDdesc *fld;
	TDesc	*TTptr;
	size_t	FLDlen;
	
	//printf("rfree(TDno = %d, FieldNo = %d)\n" ,TDno,FieldNo);

	TTptr		= &ttab[TDno];
	rtdget(TTptr);               // opens table if necessary, but can throw an assert()
	fld			= TTptr->TTfields;
	int fno		= 0;
	while ( fld->FLDelemID )
	{	
		// Pass ZERO for FieldNo frees all *non-key* fields
		// If FieldNo is non-zero, free it only if it isn't a key field or sub-rec pointer, or if it is field# 99 **** possible error ***

		if ( !FieldNo || (fno == FieldNo) || !(fld->FLDstat & fld_SUBREC) || !(fld->FLDstat & fld_KEY) )
		{
		//	fld->FLDstat = (fld->FLDstat | 4) & 0xFFE6;
			fld->FLDstat = (fld->FLDstat | fld_ZERO) & ~(fld_DATA_AVAIL|fld_SUBREC|fld_REC_LOADED);
			switch (fld->FLDtype)
			{
				case 'C':
					zap(fld->FLDdata,(fld->FLDlen + 1));	// string
					break;
				// *** LIBCL4 functionality below ***
				//case 'D':
				//case 'T':
				//	zap(fld->FLDdata,80);
				//	break;
				default:
					if ( fld->FLDlen )
						zap(fld->FLDdata,sizeof(double));	// everything else stored internally as a double
					break;
			}
		}
		fno++;
		fld++;
	}
}

// Free the *Key* fields of a TD
void rkfree(int TDno)
{
	FLDdesc *fld;
	//printf("rkfree(TDno = %d)\n" ,TDno);

	fld = ttab[TDno].TTfields;
	while ( fld->FLDelemID && fld->FLDstat & fld_KEY)	//	< 0 )// High bit set indicates Key field
//	while ( fld->FLDelemID && fld->FLDstat & < 0 ) GCC optimises &0x8000 to "< 0" for a short :o)
	{
		//fld->FLDstat = (fld->FLDstat | 4) & 0xFFE6;
		fld->FLDstat = (fld->FLDstat | fld_ZERO) & ~(fld_DATA_AVAIL|fld_SUBREC|fld_REC_LOADED);

		if ( fld->FLDtype == 'C' )
			zap(fld->FLDdata, (fld->FLDlen + 1));	// string variable
		else if ( fld->FLDlen )
			zap(fld->FLDdata,sizeof(double));		// everything else stored internally as a double

		// *****   doesn't zap Date/Time fields when in Key? (80 bytes)??
		fld++;
	}
}

// Free the *data* (non-Key) fields of a TD
void rdfree(int TDno)
{
	FLDdesc *fld;
	
	//printf("rdfree(TDno = %d)\n" ,TDno);
	fld = ttab[TDno].TTfields;
	while( fld->FLDelemID )
	{
		if ( !(fld->FLDstat & fld_KEY))	// Skip Key fields. GCC will optimise to ( >= 0 )
		{
			//fld->FLDstat = (fld->FLDstat | 0x0004) & 0xFFEE;
			fld->FLDstat |= fld_ZERO;
			fld->FLDstat &= ~(fld_DATA_AVAIL|fld_REC_LOADED);	// GCC will optimise to one instruction
		
			switch (fld->FLDtype)
			{
				case 'C':
					zap(fld->FLDdata,(fld->FLDlen + 1));		// string
					break;
				// **** LIBCL4 different ***
				//case 'D':
				//case 'T':
				//	zap(fld->FLDdata,80);
				//	break;
				default:
					if( fld->FLDlen )
						zap(fld->FLDdata, sizeof(double));		// everything else stored as double
					break;
			}
		}
		fld++;
	}
}

// Free the Workarea buffer, and mark fields as having no data loaded , only called from rdkey()
void tfree(int TDno)
{
	TDesc	*TTptr;
	FLDdesc *fld;
	char	*buffptr;
	
	//printf("tfree(TDno = %d)\n" ,TDno);

	TTptr	= &ttab[TDno];
	fld		= TTptr->TTfields;
	for ( buffptr = TTptr->TDworkarea1; fld->FLDelemID; ++fld )
	{
		zap(buffptr, fld->FLDlen);
		fld->FLDstat &= ~fld_REC_LOADED;            // clear fld_REC_LOADED flag
		buffptr += fld->FLDlen;
	}
}
#endif
