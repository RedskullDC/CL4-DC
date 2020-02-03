#ifndef STEPFWD_C
#define STEPFWD_C

#include "DBdefs.h"
#include "cl4.h"
int findnxt( TDinfo *TDptr)
{
	PAGE *PagePtr;
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
		PagePtr = _indexpg(TDptr, PageNo);
		if ( _scanpg(PagePtr, TDptr, &N1_2idx, TDptr->field_3A | 8) ) // scanpg updates N1_2idx directly
			++N1_2idx;
		
		PageNo = mstol((int *)&PagePtr->header.DataStart[4 * N1_2idx]);
		if ( N1_2idx != PagePtr->header.NumEntries )
		{
			v2 = _itosz(PagePtr, N1_2idx);
			KSize = TDptr->TDKeySize;
			if ( KSize > (unsigned int)v2 )
				KSize = v2;
			TDptr->Key1Size = cpybuf(TDptr->KeyBuf1, _itoptr(PagePtr, N1_2idx), KSize);
		}
	}
	return PageNo;
}

NODE_1*	_stepfwd(TDinfo *TDptr)
{
	int PageNo;

	//printf("_stepfwd(TDptr: x%08X)\n", TDptr);
	while ( 1 )
	{
		if ( TDptr->TDNodePtr && TDptr->TDNodePtr->NumEntries > TDptr->N1_2idx )	// Node loaded, and entries left
			return &TDptr->TDNodePtr->NODE1ptr[TDptr->N1_2idx++];					// Bump N1_2idx pointer to next record for next time

		TDptr->KeyDefSize = cpybuf(TDptr->TDKeyDefs, TDptr->KeyBuf1, TDptr->Key1Size);
		
		if ( (TDptr->TDNodePtr && !TDptr->KeyDefSize) || !(PageNo = findnxt(TDptr)))	// If Node loaded, but keysize = 0, *OR* no more pages exit out
			break;
		
		relnode(TDptr->TDNodePtr);
		TDptr->TDNodePtr = getnode(TDptr, PageNo, 0);

		_scanpg((PAGE *)TDptr->TDNodePtr, TDptr, &TDptr->N1_2idx, TDptr->field_3A);			// updates N1_2idx directly!
		TDptr->field_3A = 1;
	}
	return 0;     // \0 means we have reached the end of the line. No more data!
}

#endif
