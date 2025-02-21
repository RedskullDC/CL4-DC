#ifndef BALANCE_C
#define BALANCE_C

#include "DBdefs.h"
#include "cl4.h"

// *not* public in libcl4
void undo(DBinfo *DBptr)
{
	int *SpareList;
	int *PageList3;
	//int SpareLen;

	//printf("undo( DBptr: x%08X)\n", DBptr);

	SpareList = DBptr->SpareList;
	PageList3 = DBptr->PageList3;

	//SpareLen = length(DBptr->SpareList);
	//if ( SpareLen + length(DBptr->PageList3) <= 32 )
	
	if ( length(DBptr->SpareList) + length(DBptr->PageList3) <= 32 )
	{
		for ( ; *SpareList; ++SpareList )	//	find the end of SpareList
			;
	}
	else
	{	
		_mkfree(DBptr, SpareList);
	}

	while (*PageList3)						// Copy PageList3 entries back to SpareList. (Free Pages)
	{
		*SpareList++ = *PageList3++;
	}
	*SpareList = 0;							// Null terminate the list

	_rmlock(DBptr, DBptr->PageList2);		// PageList2 contains the list of pages currently locked. Free them all.

	DBptr->PageList3[0] = 0;	// Marked Update and Locked lists as empty
	DBptr->PageList2[0] = 0;
}

// *not* public in libcl4
void putchange(DBinfo *DBptr)
{
	int *PageList1;
	int *PageList2;

	//printf("putchange( DBptr: x%08X [ %s ])\n", DBptr, DBptr->DBname);

	PageList1 = DBptr->PageList1;
	PageList2 = DBptr->PageList2;

	//v3 = length(DBptr->PageList1);
	//if ( v3 + length(DBptr->PageList2) <= 15 )

	if ( length(DBptr->PageList1) + length(DBptr->PageList2) <= 15 )
	{
		for ( ; *PageList1; ++PageList1 )	// Find end of PageList1
			;
	}
	else
	{
		_rmlock(DBptr, PageList1);
		_mkfree(DBptr, PageList1);
	}

	while (*PageList2)					// Copy PageList2 entries back to PageList1
	{
		*PageList1++ = *PageList2++;
	}
	*PageList1 = 0;						// Null terminate the list
	
	DBptr->PageList3[0] = 0;			// Marked Update and Locked lists as empty
	DBptr->PageList2[0] = 0;
}

// *not* public in libcl4
int length(int *PageList)
{
	int *i;
	for ( i = PageList; *i; ++i )
		;
	return i - PageList;
}

bool _balance(TDinfo *TDptr, NODE *Node, int *PageList, short Depth)
{
	int v4;
	register int v5;
	int *PagePtr;
	PAGE_NODE p_pgnode;  
	char **a5;
	char *ptr;
	bool success;
	char* KeyBuff8[8];	// 0 -> 7 are valid
	short N1_2idx;

	// only called from _update()

	//printf("_balance( TDptr: x%08X, NODE: x%08X , Depth: %d)\n", TDptr,Node, Depth);

	success = true;

	ptr = ealloc(8 * TDptr->TDKeySize, 0);
	v5 = TDptr->TDKeySize;
	for ( v4 = 0 ; v4 <= 7; v4++)
	{
		KeyBuff8[v4] = &ptr[v5 * v4];    // Array of 8 KeySizeBuffers
	}
	
	a5 = KeyBuff8;
	PagePtr = &PageList[Depth];
	while ( 1 )
	{
		if ( PagePtr == PageList )		// Number of records exceeds current Tree Depth. Add an extra Level here
		{
			if ( Depth > 5 )			// If Depth is already 6, this is as big as Table can get!
				derror(15, 0, TDptr);

			putnode(TDptr, Node);
			_whead(TDptr->TDDBinfo, TDptr->TDindexOff, Node->PageNo, Depth + 1);
			goto FINISH;					// exit_success
		}
		--PagePtr;

		if ( PagePtr == PageList && !Node->NumEntries && (!(TDptr->TDFlags & 0x20) || Depth != 1) )
		{	
			// Deleting entries here, and a smaller tree is required. Decrease Depth in Table header
			_addlist(TDptr->TDDBinfo->PageList2, Node->PageNo);
			_whead(TDptr->TDDBinfo, TDptr->TDindexOff, Node->NODE2ptr->PageNo, Depth - 1);
			goto FINISH;					// exit_success
		}

		if ( Node->DataEnd <= TDptr->TDDBinfo->DBpgsize && (PagePtr == PageList || TDptr->HalfPageSize <= Node->DataEnd) )
		{
			// Node will fit in a page *AND* ( We are at deepest level *OR* Page is more than half full.)
			putnode(TDptr, Node);
			_lockpg(TDptr->TDDBinfo, Node->PageNo, 0);
			goto FINISH;					// exit_success
		}

		if ( PagePtr == PageList )
		{
			p_pgnode.NODE = freshnode(TDptr, 2);
			N1_2idx = 0;
			goto LABEL_25;
		}
		if ( !_lockpg(TDptr->TDDBinfo, *(PagePtr - 1), 2) )
			break;
		
		p_pgnode.NODE = getnode(TDptr, *(PagePtr - 1), 2);
		if ( _scanpg(p_pgnode, TDptr, &N1_2idx, 1) )		// "1" indicates to _scanpg this is a NODE structure
			++N1_2idx;
		
		if ( p_pgnode.NODE->NODE2ptr[N1_2idx].PageNo != Node->PageNo )
			derror(19, 0, TDptr);

LABEL_25:

		if (a5 == &KeyBuff8[8])		// [8] is outside the array!!
			a5 = KeyBuff8;			// go back around to the start
 
		// if Node wont fit in a page, or couldn't be spread across two nodes?
		if ( TDptr->Rec_plus_DB < Node->DataEnd || !_spread(TDptr, Node, p_pgnode.NODE, N1_2idx, a5) )
		{
			a5 += 2;
			if (a5 == &KeyBuff8[8])		// [8] is outside the array!!
				a5 = KeyBuff8;

			if ( !_split(TDptr, Node, p_pgnode.NODE, N1_2idx, a5) )
			{
				_lockpg(TDptr->TDDBinfo, *(PagePtr - 1), 0);
				Node = p_pgnode.NODE;
				goto FINISH;				// exit_success
			}
		}
		a5 += 2;
		Node = p_pgnode.NODE;
	}
	success = false;		// error_exit, something went wrong!

FINISH:
	nfree(ptr, 0);
	if ( success )
	{
		putchange(TDptr->TDDBinfo);						// all ok, put changes to DB
	}
	else
	{
		_lockpg(TDptr->TDDBinfo, Node->PageNo, 0);
		undo(TDptr->TDDBinfo);							// error somewhere, back out any changes
	}
	relnode(Node);
	return success;
}


#endif

