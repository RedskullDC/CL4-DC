#ifndef INDEX_FETCH_C
#define INDEX_FETCH_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

// Increasing fetch [see also _dfetch()]

short _ifetch(char *Dest, DPOS *dpos, TDinfo *TDptr)
{
	NODE	*node;
	PAGE	*page;
	NODE_1	*N1p;
	short	v9;
	
	//printf("_ifetch(Dest : x%08X, DPOS : x%08X, TDptr :x%08X)\n",Dest,dpos,TDptr);

	node = TDptr->TDNodePtr;
	if ( node && node->PageNo != dpos->PageNo  )
	{
		relnode(node);
		TDptr->TDNodePtr = 0;
	}

	if ( dpos->PageNo )
		goto LABEL_17;

	if ( dpos->field_8 )
	{
		//++fcount;	// fetch count? Only ever accessed from here.
		page = _indexpg(TDptr, dpos->field_8);
		dpos->PageNo = mstol((int*)&page->header.DataStart[4 * dpos->NumEntries++]);
		if ( page->header.NumEntries < dpos->NumEntries )
		{
			dpos->field_8 = mstol((int*)&page->pgData.field_16);
			dpos->NumEntries = 0;
		}
	}

//-----------------------------------  
	if ( dpos->PageNo )
	{
LABEL_17:
		if ( !TDptr->TDNodePtr )
			TDptr->TDNodePtr = getnode(TDptr, dpos->PageNo, 0);
		node = TDptr->TDNodePtr;

		N1p = &node->NODE1ptr[++dpos->field_4];
		
		v9 = cpybuf(Dest, N1p->Data, N1p->Size);
		
		if ( node->NumEntries <= dpos->field_4 )
		{
			relnode(TDptr->TDNodePtr);
			TDptr->TDNodePtr = 0;
			dpos->PageNo = 0;
			dpos->field_4 = 0;
		}
	}
	else
	{
		v9 = 0;
	}
	return v9;
}

#endif
