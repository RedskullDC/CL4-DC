#ifndef STEPBACK_C
#define STEPBACK_C

#include "DBdefs.h"
#include "cl4.h"

// *not* public
unsigned int findprev(TDinfo *TDptr)
{
	PAGE *PagePtr;
	int v2;
	//char *v3;
	int v4;
	short Depth;
	int PageNo;
	short N1_2idx;

	//printf("findprev(TDptr: x%08X)\n" ,TDptr);
	PageNo = 0;
	Depth = _rhead(TDptr->TDDBinfo, TDptr->TDindexOff, &PageNo);
	for ( TDptr->Key2Size = 0; Depth > 1; --Depth )
	{
		PagePtr = _indexpg(TDptr, PageNo);
		_scanpg((PAGE *)PagePtr, TDptr, &N1_2idx, TDptr->field_42 | 8);	// scanpg updates N1_2idx directly. |8 means PAGE* passed
		
		PageNo = mstol((int *)&PagePtr->header.DataStart[4 * N1_2idx--]);
		if ( N1_2idx >= 0 )
		{
			v2 = _itosz(PagePtr, N1_2idx);
			v4 = TDptr->TDKeySize;
			if ( v4 > (unsigned int)v2 )
				v4 = v2;
			TDptr->Key2Size = cpybuf(TDptr->KeyBuf2, _itoptr(PagePtr, N1_2idx), v4);
		}
	}
	return PageNo;
}

NODE_1* _stepbak(TDinfo *TDptr)
{
	int PageNo; // edi@5

	while ( 1 )
	{
		if ( TDptr->TDNodePtr && TDptr->N1_2idx > 0 )
			return &TDptr->TDNodePtr->NODE1ptr[--TDptr->N1_2idx];	// pre-dec N1_2idx pointer when searching downwards

		TDptr->KeyDefSize = cpybuf(TDptr->TDKeyDefs, TDptr->KeyBuf2, TDptr->Key2Size);

		if ( (TDptr->TDNodePtr && !TDptr->KeyDefSize) || !(PageNo = findprev(TDptr)))	// If Node loaded, but keysize = 0, *OR* no more pages exit out
			break;

		relnode(TDptr->TDNodePtr);
		TDptr->TDNodePtr = getnode(TDptr, PageNo, 0);

		_scanpg((PAGE*)TDptr->TDNodePtr, TDptr, &TDptr->N1_2idx, TDptr->field_42);	// updates N1_2idx directly!
		TDptr->field_42 = 1;
	}
	return 0;	// \0 means we have reached the end of the line. No more data!
}
#endif
