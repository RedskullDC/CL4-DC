#ifndef SPREAD_C
#define SPREAD_C

#include "DBdefs.h"
#include "cl4.h"

// not public in libcl4
bool adopt(TDinfo *TDptr, NODE *ND_parent, short N1_2idx, NODE *Node, NODE *Node2, char *RecBuffer)
{
	short NewRecSize;
	bool Adopted = true;	// default success.

	//printf("adopt(TDptr: x%08X, ND_parent: x%08X, N1_2idx = %d , Node: x%08X, Node2: x%08X, RecBuffer = x%08X)\n",TDptr,ND_parent,N1_2idx,Node, Node2,RecBuffer);

	NewRecSize = _promote(RecBuffer, Node, Node2, ND_parent->PageType & 1, TDptr);
	modpage(ND_parent, N1_2idx, RecBuffer, NewRecSize);
	
	if ( Node->DataEnd > TDptr->TDDBinfo->DBpgsize )	// Left node too big for a page?
		Adopted = false;
	else
		ND_parent->NODE2ptr[N1_2idx].PageNo = _repos(TDptr, Node);
	
	if ( Node2->DataEnd > TDptr->TDDBinfo->DBpgsize )	// Right node too big for a page?
		Adopted = false;
	else
		ND_parent->NODE2ptr[N1_2idx + 1].PageNo = _repos(TDptr, Node2);

	return Adopted;
}

bool _spread(TDinfo *TDptr, NODE *a2, NODE *ND_parent, short N1_2idx, char **KeyBuff)
{
	// KeyBuff == char*[2]  each is TDptr->KeySize long 

	bool v5; // edi@1
	NODE_1 *N1p; // edx@4
	NODE *NDtmp;

	//printf("_spread(TDptr: x%08X, Node: x%08X, ND_Parent: x%08X, N1_2idx: %d, KeyBuff = x%08X)\n",TDptr,a2,ND_parent,N1_2idx,KeyBuff);
	v5 = 0;
	if ( N1_2idx && _lockpg(TDptr->TDDBinfo, ND_parent->NODE2ptr[N1_2idx - 1].PageNo, 2) )	// Get node for index page which precedes node a2
	{
		NDtmp = getnode(TDptr, ND_parent->NODE2ptr[N1_2idx - 1].PageNo, a2->PageType);
		if ( a2->PageType & 2 )
		{
			N1p = &ND_parent->NODE1ptr[N1_2idx - 1];
			addpage(NDtmp, NDtmp->NumEntries, N1p->Data, N1p->Size);
			NDtmp->NODE2ptr[NDtmp->NumEntries].PageNo = a2->NODE2ptr->PageNo;
		}

		if ( a2->DataEnd + NDtmp->DataEnd > TDptr->TDDBinfo->DBpgsize )		// Are both nodes bigger than a single page?
		{
			if ( _redistrib(NDtmp, a2, TDptr->TDDBinfo->DBpgsize) )			// Returns number of recs shifted. May be -ve . 
				v5 = adopt(TDptr, ND_parent, N1_2idx - 1, NDtmp, a2, *KeyBuff);
			else
				_lockpg(TDptr->TDDBinfo, NDtmp->PageNo, 0);
		}
		else		// No, both nodes will fit in one page
		{
			moveleft(NDtmp, a2, a2->NumEntries);							// Move all entries to New node
			delpage(ND_parent, N1_2idx - 1);								// Remove from Parent index page
			ND_parent->NODE2ptr[N1_2idx - 1].PageNo = _repos(TDptr, NDtmp);
			_addlist(TDptr->TDDBinfo->PageList2, a2->PageNo);				// Mark page to be returned to system.
			v5 = 1;
		}
		relnode(NDtmp);
	}
	if ( v5 )
	{
		relnode(a2);
		return v5;
	}

	if ( ND_parent->NumEntries != N1_2idx && _lockpg(TDptr->TDDBinfo, ND_parent->NODE2ptr[N1_2idx + 1].PageNo, 2) )
	{
		NDtmp = getnode(TDptr, ND_parent->NODE2ptr[N1_2idx + 1].PageNo, a2->PageType);	// Get node for index page which follows node a2
		if ( a2->PageType & 2 )
		{
			addpage(NDtmp, 0, ND_parent->NODE1ptr[N1_2idx].Data, ND_parent->NODE1ptr[N1_2idx].Size);
			NDtmp->NODE2ptr->PageNo = a2->NODE2ptr[a2->NumEntries].PageNo;
		}

		if ( a2->DataEnd + NDtmp->DataEnd > TDptr->TDDBinfo->DBpgsize )		// Are both nodes bigger than a single page?
		{
			if ( _redistrib(a2, NDtmp, TDptr->TDDBinfo->DBpgsize) )			// Returns number of recs shifted. May be -ve
				v5 = adopt(TDptr, ND_parent, N1_2idx, a2, NDtmp, KeyBuff[1]);
			else
				_lockpg(TDptr->TDDBinfo, NDtmp->PageNo, 0);
		}
		else		// No, both nodes will fit in one page
		{
			moveright(a2, NDtmp, a2->NumEntries);							// Move all entries to New node
			delpage(ND_parent, N1_2idx);									// Remove from Parent index page
			ND_parent->NODE2ptr[N1_2idx].PageNo = _repos(TDptr, NDtmp);
			_addlist(TDptr->TDDBinfo->PageList2, a2->PageNo);				// Mark page to be returned to system.
			v5 = 1;
		}
		relnode(NDtmp);
	}
	if ( v5 )
		relnode(a2);
	return v5;
}


#endif
