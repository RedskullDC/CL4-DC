#ifndef UPDATE_C
#define UPDATE_C

#include "DBdefs.h"
#include "cl4.h"

// #define	upd_delete		0	// delete record
// #define	upd_put			1	// put record
// #define	upd_lock		2	// lock record	( create a lock record in TD2 )


bool _update(TDinfo *TDptr, char *WorkArea, POS *Pos, short UpdateMode, short *a5)
{
	NODE	*NodePtr;
	char	*RecBuffer; // [sp+2Ch] [bp-5Ch]@1
	int		PID;
	int		v10;
	short	Depth;
	short	v11;
	bool	result;
	bool	TableLocked;
	bool	RecExists;
	short	N1_2idx;
	int		PageList[6];

	//printf("_update(TDptr: x%08X,WorkArea: x%08X,POS: x%08X, UpdateMode: x%02X, a5: x%08X)\n" ,TDptr,WorkArea,Pos,UpdateMode,a5);

	TableLocked = false;
	RecExists = false;
	
	RecBuffer = ealloc(TDptr->TDRecSize, 0);
	TDptr->KeyDefSize = rtokey(TDptr->TDKeyDefs, WorkArea, Pos, TDptr->TableDefs);
	//printf("_update : KeyDefSize = %d\n",TDptr->KeyDefSize);
	while ( 1 )
	{
		while ( 1 )
		{
			emark(TDptr->TDDBinfo, TDptr->TDindexOff);
			Depth = _getpath(PageList, TDptr);				// getpath updates PageList array. PageList[Depth -1] is where record belongs (may not exist);
			//printf("_update : _getpath returned = %d\n",Depth);
			if ( Depth )
				break;							// Depth != 0 mean table had some entries. go process the command

			if ( UpdateMode == upd_delete )		// 0  == delete, and Depth == 0 [table is empty]
				goto LABEL_33;					// nothing to do, jump straight to the end

			if ( TableLocked )
				break;

			TableLocked = _locktbl(TDptr->TDDBinfo, TDptr->TDindexOff, 1);	// arrive here if table in question had no entries?
			if ( !TableLocked )
				goto LABEL_32;		// couldn't lock this table, go around again until we can
		}
		if ( Depth <= 0 || _lockpg(TDptr->TDDBinfo, PageList[Depth - 1], 2) )
			break;
LABEL_32:
		ewait(TDptr->TDDBinfo, TDptr->TDindexOff);
	}
//----------------
  
	if ( Depth )		// Depth != 0 means Table contains entries
	{
		NodePtr = getnode(TDptr, PageList[Depth - 1], 0);
		RecExists = _scanpg((PAGE*)NodePtr, TDptr, &N1_2idx, 1);	// scanpg takes NODE or PAGE structure. [ "1" signifies NODE structure]
		//printf("_update : RecExists = %s\n", RecExists ? "true":"false");
	}
	else
	{
		NodePtr = freshnode(TDptr, 0);	// Skip scanning for where record should be.
		N1_2idx = 0;					// first record in new Node
	}
	
	//	can't get here when ( UpdateMode == upd_delete && Depth == 0 [table was empty]) ie. freshnode() was called above

	if ( UpdateMode == upd_delete )		// 0  == delete
	{
		if ( RecExists )
		{
			delpage(NodePtr, N1_2idx);		// record deleted, _balance() required below
		}
		else
		{
			_lockpg(TDptr->TDDBinfo, NodePtr->PageNo, 0);
			relnode(NodePtr);
			goto LABEL_33;					// no _balance() required, as no record deleted
		}
	}
	else if ( UpdateMode == upd_lock )	// 2  == lock
	{
		if ( RecExists )
		{
			PID = mstol((int *)(NodePtr->NODE1ptr[N1_2idx].Data + 129));	// Data points at a TD2REC structure. Get the PID of this lock record
			if ( PID == getpid() )											// Does the PID in the existing lock record belong to us?
				RecExists = RecExists == false;								// "flip" value of RecExists. Allows us to mod record if *we* own the lock
			_lockpg(TDptr->TDDBinfo, NodePtr->PageNo, 0);
			relnode(NodePtr);
			goto LABEL_33;			// no _balance() required, as no record deleted
		}
		else
		{
			v10 = rtotup(RecBuffer, WorkArea, Pos, TDptr->TableDefs);
			addpage(NodePtr, N1_2idx,RecBuffer, v10);						// lock record added, _balance() required 
		}
	}
	else if ( UpdateMode == upd_put )		// upd_put && RecExists == true
	{
		if ( RecExists )
		{
			v11 = _uptuple(RecBuffer, NodePtr->NODE1ptr[N1_2idx].Data, WorkArea, Pos, TDptr->TableDefs, a5);
			modpage(NodePtr, N1_2idx, RecBuffer, v11);
		}
		else								// upd_put && RecExists == false
		{
			v10 = rtotup(RecBuffer, WorkArea, Pos, TDptr->TableDefs);
			addpage(NodePtr, N1_2idx,RecBuffer, v10);
		}
	}

	if ( !_balance(TDptr, NodePtr, PageList, Depth) )		// Normally a serious error if this fails......
		goto LABEL_32;

	if ( TableLocked )
		_locktbl(TDptr->TDDBinfo, TDptr->TDindexOff, 0);	// Free the table entry before exiting

LABEL_33:
	ewake(TDptr->TDDBinfo, TDptr->TDindexOff);
	nfree(RecBuffer, 0);
	
	result = RecExists;
	if ( UpdateMode == upd_lock )	// 2  == lock
		result = RecExists == false;
	return result;
}

#endif
