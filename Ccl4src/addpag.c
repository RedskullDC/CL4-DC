#ifndef ADDPAGE_C
#define ADDPAGE_C

#include "DBdefs.h"
#include "cl4.h"

int addpage(NODE *Node, short N1_2idx, char *RecBuffer, int RecSize)
{
	int LastEntry;
	NODE_1 *v5;
	NODE_1 *N1p;
	NODE_1 *v7;
	NODE_2 *N2p;
	int result;
	
	//addpage() can either insert regular data or Page Index data.

	//printf("addpage(NODE: x%08X,N1_2idx: %d,RecBuffer: x%08X, RecSize: %d)\n" ,Node,N1_2idx,RecBuffer,RecSize);

	_chkitems(Node, 1);						// Ensures Node can hold an extra entry, and re-allocates as necessary

	LastEntry = Node->NumEntries;
	//printf("addpage : LastEntry = %d\n",LastEntry);
	v5	= Node->NODE1ptr;
	N1p = &v5[LastEntry];
	N2p = &Node->NODE2ptr[LastEntry + 1];
	v7	= &v5[N1_2idx];	// insert position

	// make room at position N1_2idx, Move all higher entries up one position
	while ( v7 < N1p )
	{
		N1p->Data	= N1p[-1].Data;
		N1p->Size	= N1p[-1].Size;
		N2p->PageNo = N2p[-1].PageNo;
		--N1p;
		--N2p;
	}
	// N1p, N2p are now pointing at the insert position.
	N1p->Data = RecBuffer;
	N1p->Size = RecSize;
	N2p->PageNo = N2p[-1].PageNo;

	result = RecSize + Node->field_16;	// ??
	Node->DataEnd += result;
	++Node->NumEntries;
	return result;
}

#endif
