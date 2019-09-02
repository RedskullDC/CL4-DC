#ifndef READNEXT_C
#define READNEXT_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"	// for bit field values

void fix_buf(int TTno, char *WorkArea)
{
	FLDdesc *fld;
	char *v3;
	int v4;
	int v7;

	// convert all string key fields to uppercase for comparison  
	for ( fld = ttab[TTno].TTfields; fld->FLDstat & fld_KEY; fld++ )
	{
		if ( fld->FLDtype == 'C' )
		{
			v3 = WorkArea;
			v7 = 0;
			while ( *v3 && fld->FLDlen > v7 )
			{
				v4 = *v3;
				if (islower(v4))
					*v3 = toupper(v4);	// Force all string entries to upper case for comparisons
				v7++;
				v3++;
			}
		}
		WorkArea += fld->FLDlen;
	}
}

short rdnxt(int TDno, EXPR *expr, int Lock, int a4)
{
    char	*Workarea1;
	TDesc	*TTptr;
	int		MaxLen;
	int		TDNo;
	short	v7;
	char	Src[1024];
	
	//printf("rdnxt(TDno = %d, EXPR = x%08X, Lock = %d, a4 = %d)\n",TDno,expr,Lock,a4);

    TTptr		= &ttab[TDno];
    TDNo		= rtdget(TTptr);
    Workarea1	= TTptr->TDworkarea1;
    ulckrec(TDno);
    rfree(TDno, a4 ? 99 : 0);			// This will break records with > 98 fields if a4 is set!

   	TTptr->TDlocked &= ~ttl_AVAIL;		// 0xFFDFu 1111-1111-1101-1111		  clear ttl_AVAIL flag
    if ( TTptr->TDlocked & ttl_CREATE )	// 0x0200	don't allow lock on temp tables we created. DOH!
        Lock = 0;

	while ((v7 = getnxt(TDNo, Workarea1, expr)) > 0 )			// (< 0) == ERROR, 0 = no data, (> 0) == success
	{
        if ( !Lock )		// Data found, if no lock requested, return to caller
		{
            rkfree(TDno);
            ravail(TDno);	// Set flags indicating data is in WorkArea buffer
            break;
		}
        
		MaxLen = keysize(TDNo);
        cpybuf(Src, Workarea1, MaxLen);		// Save key data for later
        fix_buf(TDno, Workarea1);			// set to all upper case for key comparison

        while ((v7 = lgetr(TDNo, Workarea1, 0)) == -10 && Lock != 1024)
            sleep(1u);						// Keep trying until record becomes free. Deadlock!

        if ( !v7 )		// very difficult to get here. Means getnxt() found some data, but it disappeared  :o(
		{
			v7 = ulockr(TDNo, Workarea1, 0);
			if ( v7 < 0 )
				dberror(v7, 0, TDNo);
		}
		else if (v7 < 0 && v7 != -10)
		{
			break;
		}
		else // v7 > 0 == success or v7 = -10, record was locked but we used a lockr request
        {
			cpybuf(Workarea1, Src, MaxLen);	// copy orginal key back (without case conversion)
			if ( v7 > 0 )				// == no error
			{
				TTptr->TDlocked |= ttl_LOCK;	// 0x0001u;
				cpybuf(TTptr->TDworkarea2, Workarea1, TTptr->TDrecsize);
            }
            rkfree(TDno);
            ravail(TDno);
            break;
        }
    }
//================================
    if ( v7 < 0 && v7 != -10 )	// -10 == record locked, not an error as such
        dberror(v7, 0, TDNo);
    return v7;
}

#endif

