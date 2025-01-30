#ifndef GETR_C
#define GETR_C

#include "DBdefs.h"
#include "cl4.h"

// ** not public in libcl4 **

int _getr(int TDNo, char *WorkArea1, char *WorkArea2, POS *Pos, int Lock)
{
	TDinfo *TDptr;
	PAGE_NODE p_pgnode;
	int v7;
	short Depth;
	int PageList[6];
	short N1_2idx;

	//printf("_getr( TDno: x%04X (%3d),WorkArea1 = %s, WorkArea2 = %s, POS = x%08X, Lock: %d)\n", TDNo,TDNo,WorkArea1,WorkArea2,Pos,Lock);
	TDptr = _chktd(TDNo);
	if ( !(TDptr->TDFlags & 8) )
		derror(36, 0, TDptr);

	TDptr->KeyDefSize = rtokey(TDptr->TDKeyDefs, WorkArea2, Pos, TDptr->TableDefs);
	
	if ( !Lock || (v7 = -10, _lockr(TDptr->TDDBinfo, TDptr->TDindexOff, TDptr->TDKeyDefs, TDptr->KeyDefSize, 2)) )
	{
		Depth = _getpath(PageList, TDptr);	// updates PageList[] directly
		v7 = 0;
		if ( Depth )
		{
			p_pgnode.PAGE = _datapg(TDptr, PageList[Depth - 1]);
			v7 = _scanpg(p_pgnode, TDptr, &N1_2idx, 9);	// updates N1_2idx directly
			if ( v7 )
				tuptor(WorkArea1, Pos, _itoptr(p_pgnode.PAGE, N1_2idx), TDptr->TableDefs);
		}
	}
	return v7;
}

// ** HACK! Should be called through xenter()

short getr(int TDno, char *WorkArea, POS *Pos)
{
	return _getr(TDno, WorkArea, WorkArea, Pos, 0);
}

short lgetr(int TDNo, char *WorkArea1, POS *Pos)
{
	return _getr(TDNo, WorkArea1, WorkArea1, Pos, 1);
}

short retrieve(int TDno, char *WorkArea1, char *WorkArea2)
{
	//return xenter(getr, TDno, WorkArea1, WorkArea2, 0, 0);
	return _getr(TDno, WorkArea1, WorkArea2, 0, 0);
}

short lretrieve(int TDno, char *WorkArea1, char *WorkArea2)
{
	//return xenter(getr, TDno, WorkArea1, WorkArea2, 0, 0);
	return _getr(TDno, WorkArea1, WorkArea2, 0, 1);
}


#endif
