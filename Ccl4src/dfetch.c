#ifndef DFETCH_C
#define DFETCH_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

// Descending fetch [see also _ifetch()]

short _dfetch(char *Dest, DPOS *dpos, TDinfo *TDptr)
{
	PAGE	*page;
	NODE	*node;
	NODE_1	*N1p;
	short	v9;
	
	//printf("_dfetch(Dest : x%08X, DPOS : x%08X, TDptr :x%08X)\n",Dest,dpos,TDptr);

	node = TDptr->TDNodePtr;
	if ( node && node->PageNo != dpos->PageNo )
	{
		relnode(node);
		TDptr->TDNodePtr = 0;
	}

	if ( dpos->PageNo )
		goto LABEL_21;
	if ( dpos->field_8 )
	{
		page = _indexpg(TDptr, dpos->field_8);
		if ( !dpos->NumEntries )
			dpos->NumEntries = page->header.NumEntries + 1;
		
		--dpos->NumEntries;
		
		dpos->PageNo = mstol((int*)&page->header.DataStart[4 * dpos->NumEntries]);
		
		if ( !dpos->NumEntries )
			dpos->field_8 = mstol((int*)&page->pgData.field_12);
	}

	if ( dpos->PageNo )
	{
LABEL_21:
		if ( !TDptr->TDNodePtr )
			TDptr->TDNodePtr = getnode(TDptr, dpos->PageNo, 0);
		node = TDptr->TDNodePtr;

		if ( !dpos->field_4 )
			dpos->field_4 = node->NumEntries;

		N1p = &node->NODE1ptr[--dpos->field_4];
		
		v9 = cpybuf(Dest, N1p->Data, N1p->Size);
		
		if ( !dpos->field_4 )
		{
			relnode(TDptr->TDNodePtr);
			TDptr->TDNodePtr = 0;
			dpos->PageNo = 0;
		}
	}
	else
	{
		v9 = 0;
	}
	return v9;
}


#endif
