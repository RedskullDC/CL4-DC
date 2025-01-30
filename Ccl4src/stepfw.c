#ifndef STEPFWD_C
#define STEPFWD_C

#include "DBdefs.h"
#include "cl4.h"
int findnxt( TDinfo *TDptr)
{
	PAGE_NODE pgnode;
	int v2;
	int KSize;
	short Depth;
	int PageNo;
	short N1_2idx;

	//printf("findnxt(TDptr: x%08X)\n", TDptr);
	PageNo	= 0;
	Depth	= _rhead(TDptr->TDDBinfo, TDptr->TDindexOff, &PageNo);
	for ( TDptr->Key1Size = 0; Depth > 1; --Depth )
	{
		pgnode.PAGE = _indexpg(TDptr, PageNo);
		if ( _scanpg(pgnode, TDptr, &N1_2idx, TDptr->field_3A | 8) ) // scanpg updates N1_2idx directly
			++N1_2idx;
		
		PageNo = mstol((int *)&pgnode.PAGE->header.DataStart[4 * N1_2idx]);
		if ( N1_2idx != pgnode.PAGE->header.NumEntries )
		{
			v2 = _itosz(pgnode.PAGE, N1_2idx);
			KSize = TDptr->TDKeySize;
			if ( KSize > (unsigned int)v2 )
				KSize = v2;
			TDptr->Key1Size = cpybuf(TDptr->KeyBuf1, _itoptr(pgnode.PAGE, N1_2idx), KSize);
		}
	}
	return PageNo;
}

NODE_1*	_stepfwd(TDinfo *TDptr)
{
	int PageNo;
	PAGE_NODE pgnode;

	//printf("_stepfwd(TDptr: x%08X)\n", TDptr);
	while ( 1 )
	{
		if ( TDptr->TDNodePtr && TDptr->TDNodePtr->NumEntries > TDptr->N1_2idx )	// Node loaded, and entries left
			return &TDptr->TDNodePtr->NODE1ptr[TDptr->N1_2idx++];					// Bump N1_2idx pointer to next record for next time

		TDptr->KeyDefSize = cpybuf(TDptr->TDKeyDefs, TDptr->KeyBuf1, TDptr->Key1Size);
		
		if ( (TDptr->TDNodePtr && !TDptr->KeyDefSize) || !(PageNo = findnxt(TDptr)))	// If Node loaded, but keysize = 0, *OR* no more pages exit out
			break;
		
		relnode(TDptr->TDNodePtr);
		
		pgnode.NODE = getnode(TDptr, PageNo, 0);
		TDptr->TDNodePtr = pgnode.NODE;
		
		_scanpg(pgnode, TDptr, &TDptr->N1_2idx, TDptr->field_3A);			// updates N1_2idx directly!
		TDptr->field_3A = 1;
	}
	return 0;     // \0 means we have reached the end of the line. No more data!
}

#endif
