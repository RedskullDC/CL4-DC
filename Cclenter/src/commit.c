#ifndef COMMIT_C
#define COMMIT_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

short _commit(int TDno, bool FreeSbuf)
{
    TDinfo	*TDptr;
	int		v4;
	int		PageNo;
	short	result;
	short	v6;
	short	Depth;
	RLIST	rlst[6];
	
    TDptr = _chktd(TDno);
    v4 = check_bup_sem(TDptr->TDDBinfo);		// 0 == success
    if ( v4 )
        derror(v4, 0, TDptr);                   // 
                                                // 
    if ( TDptr->TDSallocBuf && !(TDptr->TDFlags & 2) )	// bit 2 indicates that TDSallocBuf is holding a RLARR* !
    {
        v6 = _sflush(rlst, 1, TDptr->TDSallocBuf, TDptr);								// MaxDepth of 1
        if ( v6 )
        {
            if ( !_locktbl(TDptr->TDDBinfo, TDptr->TDindexOff, 1) )						// couldn't lock the table header for writing. error!
                derror(18, 0, TDptr); 

            Depth = _rhead(TDptr->TDDBinfo, TDptr->TDindexOff, &PageNo);				// Read old table data, updates PageNo
            _whead(TDptr->TDDBinfo, TDptr->TDindexOff, rlst[0].PageNo, rlst[0].Depth);	// Save new data to Table header
            _locktbl(TDptr->TDDBinfo, TDptr->TDindexOff, 0);							// Unlock table header
            _sparetree(TDptr, PageNo, Depth);											// Return old table data pages to system
        }
        if ( FreeSbuf )
            TDptr->TDSallocBuf = (SALLOCBUF *)nfree(TDptr->TDSallocBuf, 0);// 
                                                // 
        return v6;
    }
    else
    {
        return 0;
    }
}

short commit(int TDno)
{
	//return (signed short)xenter((int)commit, a1, 1);
	return _commit(TDno, true);
}

#endif
