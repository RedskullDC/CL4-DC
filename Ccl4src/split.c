#ifndef SPLIT_C
#define SPLIT_C

#include "DBdefs.h"
#include "cl4.h"

int _split(TDinfo *TDptr, NODE *Node, NODE *ND_parent, short N1_2idx, char **KeyBuff)
{
	NODE *NewNODE;
	int v6;
	short KeyIndex;

   //printf("_split(TDptr: x%08X, Node: x%08X, ND_parent: x%08X,N1_2idx: %d, KeyBuff = x%08X)\n",TDptr,Node,ND_parent,N1_2idx,KeyBuff);

	KeyIndex = 0;
	if ( TDptr->TDDBinfo->DBpgsize < Node->DataEnd )// Node data won't fit in one page!!
	{
		do
		{
			NewNODE = freshnode(TDptr, Node->PageType);
			if ( NewNODE->DataEnd < TDptr->HalfPageSize || TDptr->TDDBinfo->DBpgsize < Node->DataEnd )
			{
				while ( TDptr->TDDBinfo->DBpgsize >= (Node->NODE1ptr[Node->NumEntries - 1].Size + NewNODE->DataEnd + NewNODE->field_16) )
				{
					moveright(Node, NewNODE, 1);
					if ( NewNODE->DataEnd >= TDptr->HalfPageSize && TDptr->TDDBinfo->DBpgsize >= Node->DataEnd )
							break;
				}
			}
			v6 = _promote(KeyBuff[KeyIndex], Node, NewNODE, ND_parent->PageType & 1, TDptr);
			addpage(ND_parent, N1_2idx, KeyBuff[KeyIndex], v6);
			ND_parent->NODE2ptr[N1_2idx + 1].PageNo = NewNODE->PageNo;
			putnode(TDptr, NewNODE);
			relnode(NewNODE);
			KeyIndex++;
		}
		while ( KeyIndex <= 1 && TDptr->TDDBinfo->DBpgsize < Node->DataEnd );
	}

	if ( KeyIndex )
	{
		ND_parent->NODE2ptr[N1_2idx].PageNo = _repos(TDptr, Node);
	}
	else
	{
		putnode(TDptr, Node);
		_lockpg(TDptr->TDDBinfo, Node->PageNo, 0);
	}
	relnode(Node);
	return KeyIndex;
}

#endif
