#ifndef MOVERIGHT_C
#define MOVERIGHT_C

#include "DBdefs.h"
#include "cl4.h"

int moveright(NODE *NodeSRC, NODE *NodeDEST, short EntriesToMove)
{
	//int SrcLastEntry;		
	int result;				
	short DestLastEntry;	
	short i;				
	short SizeMoved;		

	NODE_1 *N1pSrc;			
	NODE_1 *N1p;			
	NODE_1 *v6;				
	
	NODE_2 *v5;				
	NODE_2 *N2pSrc;			
	NODE_2 *N2p;

	//printf("moveright(SRC = x%08X, DEST = x%08X, Num: %d\n",NodeSRC,NodeDEST, EntriesToMove);

	_chkitems(NodeDEST, EntriesToMove);		// ensure NodeDEST can hold required number of entries.

	DestLastEntry = NodeDEST->NumEntries;
	N1p = &NodeDEST->NODE1ptr[DestLastEntry];
	v5 = &NodeDEST->NODE2ptr[DestLastEntry + 1];

	v6 = &N1p[EntriesToMove];                // Expand DestNODE by EntriesToMove.
	N2p = &v5[EntriesToMove];

	SizeMoved = 0;
	while ( DestLastEntry > 0 )             // If DestNODE empty, no need to shift entries
	{  
		//--N1p;								// make (EntriesToMove) space at start of DestNODE
		//--v6;								// shift entries up.
		(--v6)->Data = (--N1p)->Data;
		v6->Size = N1p->Size;
		//*v6 = *N1p;						// *** experimental *** not all compilers allow structure copy!!!
		//--N2p;
		//--v5;
		(--N2p)->PageNo = (--v5)->PageNo;
		--DestLastEntry;
	}
  
	N1pSrc = &NodeSRC->NODE1ptr[NodeSRC->NumEntries];	// Last entry in SRC (left) Node
	N2pSrc = &NodeSRC->NODE2ptr[NodeSRC->NumEntries];

	N2p--;
	N2p->PageNo = N2pSrc->PageNo;
	
	for ( i = EntriesToMove; i > 0; --i )            // Copy entries from end of left node to start of right node
	{	
		//--N1pSrc;
		//--v6;
		(--v6)->Data = (--N1pSrc)->Data;
		v6->Size = N1pSrc->Size;
		//--N2p;
		//--N2pSrc;
		(--N2p)->PageNo = (--N2pSrc)->PageNo;
		SizeMoved += v6->Size;
	}

	// adjust SRC/DEST node pointers,counters
	result = (SizeMoved	+ EntriesToMove * NodeDEST->field_16);
	NodeDEST->DataEnd		+= result;
	NodeDEST->NumEntries	+= EntriesToMove;
	NodeSRC->DataEnd		-= result;
	NodeSRC->NumEntries		-= EntriesToMove;
	return result;		// return size of data moved
}
#endif

