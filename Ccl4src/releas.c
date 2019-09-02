#ifndef RELEASE_C
#define RELEASE_C

#include "DBdefs.h"
#include "cl4.h"

// *** not public in libcl4() ***
void mfree(TDinfo *TDptr, SALLOCBUF *ptr)
{
    RLIST *rl; // esi@1
    short v3; // edi@2

    for ( rl = ptr->RL_Head; rl; rl = (RLIST *)nfree(rl, (void *)rl->PageNo) )
    {
        v3 = 0;
		while ( rl->Depth > v3 )
		{
			_sparetree(TDptr, rl[v3 + 1].PageNo, rl[v3 + 1].Depth);
            v3++;
        }
    }
    nfree(ptr, 0);
}

// *** not public in libcl4() ***
void sfree(TDinfo *TDptr, RLARR *RL)
{
    RLARR *RLa; // esi@1

	for ( RLa = RL; RLa; RLa = (RLARR *)nfree(RLa, RLa->NextRLARR) )
		_sparetree(TDptr, RLa->PageNo, RLa->Depth);
}

signed int _release(int TDno)
{
	TDinfo *TDptr; // esi@1
	signed int result; // eax@1
	DBinfo *DBptr; // edi@2

	TDptr = _rmtd(TDno);
	result = -1;
	if ( TDptr )
	{
		DBptr = TDptr->TDDBinfo;
		if ( TDptr->TDSallocBuf )
		{
			if ( TDptr->TDFlags & 2 )
				sfree(TDptr, (RLARR *)TDptr->TDSallocBuf);	// if bit 2 set, TDSallocBuf holds an RLARR*
			else
				mfree(TDptr, TDptr->TDSallocBuf);
		}
		if ( !(TDptr->TDFlags & 1) )
		{
			close(DBptr->DBfilehandle);
			nfree(DBptr, 0);
		}
		nfree(TDptr->TableDefs, 0);
		relseq(TDptr);
		nfree(TDptr, 0);
		result = TDno;
	}
	return result;
}

int release(int TDno)
{
	//return xenter(release, TDno, 0);
	return _release(TDno);

}
#endif
