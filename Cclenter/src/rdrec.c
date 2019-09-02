#ifndef READREC_C
#define READREC_C

#include <stdio.h>
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"	// for bit field values

int rdrec(int TDno, int Lock)
{
	TDesc	*TTptr; // esi@1
	char	*WorkArea1;
	int		result;
	int		TTno;
	int		v11;
	short	retCode;
	short	v7;
	int		KeySize;
	char	SaveK[180];

	//printf("rdrec(TDno = %d,Lock = %d)\n" ,TDno,Lock);

	v11			= 0;
	TTptr		= &ttab[TDno];
	ulckrec(TDno);
	TTno		= rtdget(TTptr);
	WorkArea1	= TTptr->TDworkarea1;

	tttorec(TDno, WorkArea1, 1);
	rdfree(TDno);

   	TTptr->TDlocked &= ~ttl_AVAIL;					// 0xFFDFu 1111-1111-1101-1111		  clear ttl_AVAIL flag
    if ( TTptr->TDlocked & ttl_CREATE )				// 0x0200	don't allow lock on temp tables we created. DOH!
        Lock = 0;

	retCode = getr(TTno, WorkArea1, 0);
	if ( retCode >= 0 )
	{
		if ( Lock )									// Get record with a LOCK
		{
			KeySize = keysize(TTno);
			if ( retCode != 1 )
				tttorec(TDno, WorkArea1, 2);		// 2 : copy only *Key* fields, no case conversion.

			cpybuf(SaveK, WorkArea1, KeySize);		// Save key area
			fix_buf(TDno, WorkArea1);				// set all key string values to uppercase	
			while ( 1 )
			{
				v7 = lgetr(TTno, WorkArea1, 0);
				v11 = v7;
				if ( v7 != -10 || Lock == 1024 )	// record not locked, or we passed lockrequest (lockr)
					break;
				sleep(1u);							// Deadlock until record is unlocked by another process 
			}
			if ( v7 != -10 )
				TTptr->TDlocked |= ttl_LOCK;		// 0x0001u;

			cpybuf(WorkArea1, SaveK, KeySize);		// restore key area in case string values were modded
			cpybuf(TTptr->TDworkarea2, WorkArea1, TTptr->TDrecsize);
		}
		if (retCode == 1 )	// success. record loaded ok
		{
			rkfree(TDno);
			ravail(TDno);
		}
	}
    if ( v11 == -10 )				// record locked, and we called with lockr
		return v11;					// return -10 (locked)
    else if ( retCode >= 0 )		// success. 0 = no data, 1 = rec loaded ok
		return retCode;
    else							// If < 0, and !-10, we have a bad error
    {
		dberror(retCode, 0, TTno);
		return retCode;
    }
}
#endif
