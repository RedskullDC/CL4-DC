#ifndef DELR_C
#define DELR_C

#include "DBdefs.h"
#include "cl4.h"

// ** not public in libcl4 **

int _delr(int TDno, char *WorkArea, POS *Pos)
{
	TDinfo *TDptr;
	int v5;

	//printf("_delr(TDno: %d,WorkArea: x%08X,POS: x%08X)\n" ,TDno,WorkArea,Pos);

	TDptr = _chktd(TDno);
	
	v5 = check_bup_sem(TDptr->TDDBinfo);	// bitmap update semaphore?
	if ( v5 )
		derror(v5, 0, TDptr);
	
	if ( (TDptr->TDFlags & 0x0C) != 0x08 )
		derror(37, 0, TDptr);

	return _update(TDptr, WorkArea, Pos, 0, 0);
}
// ** HACK! Should be called through xenter()

int delr(int TDno, char *WorkArea, POS *Pos)
{
	return _delr(TDno, WorkArea, Pos);
}

int delete(int TDno, char *WorkArea)
{
	return _delr(TDno, WorkArea, 0);
}

#endif
