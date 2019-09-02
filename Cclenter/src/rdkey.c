#ifndef READKEY_C
#define READKEY_C

#include "DBdefs.h"
#include "cl4.h"


// another function called _diffkey()!!! 
// watch for name conflict

bool diffKey(int TDno) // Check to see if any Key fields have been altered since last read from disk
{
	char	*Buff;
	TDesc	*TTptr;
	FLDdesc *fld;
	bool	result;
	int		NumBytes;

	//printf("diffKey(TDno = %d)\n" ,TDno);

	NumBytes = 0;
	TTptr	 = &ttab[TDno];
	Buff	 = mmalloc(TTptr->TDrecsize + 1);		// ensure big enough to fit entire record in case all values are keys
	tttorec(TDno, Buff, 2);							// save currently loaded values to test buffer
	
	for ( fld = TTptr->TTfields; fld->FLDelemID && fld->FLDstat & fld_KEY; fld++ )
		NumBytes += fld->FLDlen;
	
	//DumpBlock(Buff, NumBytes);
	//DumpBlock(TTptr->TDworkarea1,NumBytes);

	result = cmpbuf(TTptr->TDworkarea1, Buff, NumBytes);
	mfree_0(Buff);
	//printf("diffKey : returned %s\n", result == 0 ? "true":"false");
	return result == 0;
}


void rdkey(int TDno, int NameSrch)
{
	TDesc	*TTpk;
	TDesc	*TTptr;
	FLDdesc *fld;
	int		TDky;
	bool	v10;
	int		ErrorCode;
	short	v8;

	//printf("rdkey(TDno = %d, %d)\n" ,TDno, NameSrch);

	TTptr = &ttab[TDno];
	v10 = false;

	// either no record loaded, or key fields have been changed.
	// if so, try and load record with key value that is currently loaded
	if ( !(TTptr->TDlocked & ttl_AVAIL) || diffKey(TDno))			// ttl_AVAIL == x20
	{
		v10 = true;
		tttorec(TDno, TTptr->TDworkarea1, 1);
		if ( getr( rtdget(TTptr), TTptr->TDworkarea1, 0) <= 0 )		// <=0 record didn't exist
		{
			TTptr->TDlocked &= ~ttl_AVAIL;							// clear record loaded flag.
		}
		else
		{
			ravail(TDno);											// >0 (record existed)\n");
			TTptr->TDlocked |= ttl_AVAIL;							// set record loaded flag.
		}
	}
//------------------------

	for ( fld = TTptr->TTfields; fld->FLDelemID; ++fld )
	{
		if ( fld->FLDtype == 'K' )
		{
//TDky = findtd(TTptr->DBcontext, (NameSrch ? fld->FLDname : fld->FLDdata), TTptr->DBnumber);	// libcl4.  No Context involved here, since our TD list is private.
			TDky = findtd((NameSrch ? fld->FLDname : fld->FLDdata), TTptr->DBnumber);		// NameSrch steers whether search by Name/Data. Appears unused
			if ( TDky > 0 )
			{
				rfree(TDky, 0);
				tfree(TDky);
				TTpk = &ttab[TDky];
				TTpk->TDlocked &= ~ttl_AVAIL;		// 0xFFDFu;
				if ( TTptr->TDlocked & ttl_AVAIL)	// 0x0020u
				{
					if ( tdtotd(TDky, TDno, 1, 1) )	// copy Key fields from TDno -> TDky. return true if data was copied
					{
						v8 = getr(rtdget(TTpk), TTpk->TDworkarea1, 0);
						ErrorCode = v8;
						if ( v8 <= 0 )				// error or not-found
						{
							if ( v8 < 0 )			// error
								dberror(ErrorCode, 0,rtdget(TTpk));			// dead end error
							// not found
							rfree(TDky, 0);
							tfree(TDky);
						}
						else						// record found
						{
							ravail(TDky);
							TTpk->TDlocked |= ttl_LOCK;	// 0x0001u;
							cpybuf(TTpk->TDworkarea2, TTpk->TDworkarea1, TTpk->TDrecsize);
						}
					}
				}
			}
		}
	}
	if ( v10 )
	{
		tfree(TDno);
		TTptr->TDlocked &= ~ttl_AVAIL;	// clear table loaded flag
	}
}
#endif
