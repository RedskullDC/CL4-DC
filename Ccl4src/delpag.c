#ifndef DELPAGE_C
#define DELPAGE_C

#include "DBdefs.h"
#include "cl4.h"

int delpage(NODE *Node, short N1_2idx)
{
	NODE_1 *v5;
	NODE_1 *N1p;
	NODE_1 *N1pLastEntry;
	NODE_2 *N2p;
	int v7;
	
	//delpage() can either delete regular data or Page Index data.

	//printf("delpage(NODE: x%08X,N1_2idx: %d)\n" ,Node,N1_2idx);

	v5 = Node->NODE1ptr;
	N1p = &v5[N1_2idx];	 // index of entry to remove
	N2p = &Node->NODE2ptr[N1_2idx + 1];	
	v7 = Node->field_16 + N1p->Size;
	N1pLastEntry = &v5[Node->NumEntries];

	while( N1p < N1pLastEntry )			// Move remaining entries down.
	{
		N1p->Data	= N1p[1].Data;
		N1p->Size	= N1p[1].Size;
		N2p->PageNo = N2p[1].PageNo;
		N1p++;
		N2p++;
	}

	Node->DataEnd -= v7;	// adjust total size as necessary
	--Node->NumEntries;
	return v7;
}

#endif
