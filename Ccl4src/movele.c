#ifndef MOVELEFT_C
#define MOVELEFT_C

#include "DBdefs.h"
#include "cl4.h"

int moveleft(NODE *NodeDEST, NODE *NodeSRC, short EntriesToMove)
{
	int DestLastEntry;
	short count;	
	int result;		
	short SizeMoved;

	NODE_1 *N1p;
	NODE_1 *N1pSrc;
	NODE_1 *N1pDest;
	
	NODE_2 *N2p;
	NODE_2 *N2pSrc;
	NODE_2 *N2pDest;

	//printf("moveleft( DEST = x%08X,SRC = x%08X, Num: %d\n",NodeDEST,NodeSRC, EntriesToMove);

	_chkitems(NodeDEST, EntriesToMove);	// ensure NodeDEST can hold required number of entries.
	
	N1pSrc = NodeSRC->NODE1ptr;
	N2pSrc = NodeSRC->NODE2ptr + 1;

	DestLastEntry = NodeDEST->NumEntries;
	N1pDest = &NodeDEST->NODE1ptr[DestLastEntry];
	N2pDest = &NodeDEST->NODE2ptr[DestLastEntry + 1];

	SizeMoved = 0;

	// this could be made more efficient with a block move!!!
	for (count = EntriesToMove ; count > 0; --count )		// Copy "EntriesToMove" entries from start of right node to end of left node
	{									
		SizeMoved += N1pSrc->Size;					// track the number of bytes moved
		N1pDest->Data = N1pSrc->Data;
		(N1pDest++)->Size = (N1pSrc++)->Size;
		//*(N1pDest++) = *(N1pSrc++);					// *** warning *** structure copy not always portable!!!
		
		(N2pDest++)->PageNo = (N2pSrc++)->PageNo;
		//*(N2pDest++) = *(N2pSrc++);

		//++N2pSrc;
		//++N2pDest;
		//++N1pSrc;
		//++N1pDest;
	}

	N1p = NodeSRC->NODE1ptr;
	N2p = NodeSRC->NODE2ptr;
	
	N2p->PageNo = N2pSrc[-1].PageNo;
	N2p++;
	
	
	for ( count = NodeSRC->NumEntries - EntriesToMove ; count > 0 ; --count)	// Shift remaining entries in Right NODE down
	{
		N1p->Data = N1pSrc->Data;
		(N1p++)->Size = (N1pSrc++)->Size;
		//*(N1p++) = *(N1pSrc++);						// *** warning *** structure copy not always portable!!!
		
		(N2p++)->PageNo = (N2pSrc++)->PageNo;
		//*(N2p++) = *(N2pSrc++);
		//++N2pSrc;
		//++N2p;
		//++N1pSrc;
		//++N1p;
	}
	
	// previous block could be made *much* more efficient with a block move!!!
	//count = NodeSRC->NumEntries - EntriesToMove;
	//memcpy(N1p,N1pSrc,sizeof(NODE_1) * count);
	//memcpy(N2p,N2pSrc,sizeof(NODE_2) * count);

	// adjust SRC/DEST node pointers,counters
	result = (SizeMoved + EntriesToMove * NodeDEST->field_16);
	NodeDEST->DataEnd		+= result;
	NodeDEST->NumEntries	+= EntriesToMove;
	NodeSRC->DataEnd		-= result;
	NodeSRC->NumEntries		-= EntriesToMove;
	return result;		// return size of data moved
}


#endif

