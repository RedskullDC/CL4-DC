#ifndef ULOCKR_C
#define ULOCKR_C

#include "DBdefs.h"
#include "cl4.h"

// *** not public in libcl4()
int _ulockr_0(int TDNo, char *WorkArea, POS *Pos)
{
	TDinfo	*TDptr; // eax@1
	int		KeySize; // eax@1

    //printf("_ulockr_0(TDno: %d, Workarea : x%08X, POS : x%08X)\n", TDNo, WorkArea,Pos);

	TDptr = _chktd(TDNo);
	KeySize = rtokey(TDptr->TDKeyDefs, WorkArea, Pos, TDptr->TableDefs);
	return _lockr(TDptr->TDDBinfo, TDptr->TDindexOff, TDptr->TDKeyDefs, KeySize, 0);
}


int ulockr(int TDno, char *Workarea, POS *Pos)		// *** XENTER ***
{
	//return xenter(ulockr_0, TDno, Workarea, Pos);
	return _ulockr_0(TDno, Workarea, Pos);

}
#endif
