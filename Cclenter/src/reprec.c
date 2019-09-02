#ifndef REPLACEREC_C
#define REPLACEREC_C

#include "DBdefs.h"
#include "cl4.h"

int reprec(int TDno, int InvLock)
{
	TDesc	*TTptr;
	char	*Workarea1;
	bool	v9;
	int		TTno;
	int		retcode;

	//printf("reprec(TDno = %d, InvLock = %d)\n" ,TDno, InvLock);

	TTptr		= &ttab[TDno];
	v9			= false;
	TTno		= rtdget(TTptr);
	Workarea1	= TTptr->TDworkarea1;
	tttorec(TDno, Workarea1, 0);

	if ( (TTptr->TDlocked & 0x21) == 0x21 && (v9 = true, cmpbuf(TTptr->TDworkarea2, Workarea1, TTptr->TDrecsize)) )
	{
		if ( InvLock )
			ulckrec(TDno);
		return 0;
	}
	else
	{
		retcode = clputr(TTno, Workarea1, 0, TTptr, (v9 ? TTptr->TDworkarea2 : 0));
		if ( retcode < 0 )
			dberror(retcode, 0, TTno);			// dead end error
		
		if ( TTptr->TDlocked & ttl_LOCK);		// 0x0001
		{
			if ( InvLock )
				ulckrec(TDno);
			else
			{
				if ( cmpbuf(TTptr->TDworkarea2, Workarea1, keysize(TTno)) )
					cpybuf(TTptr->TDworkarea2, Workarea1, TTptr->TDrecsize);
			}
		}
		TTptr->TDlocked |= ttl_AVAIL;	// 0x0020u
		return retcode;
	}
}
#endif
