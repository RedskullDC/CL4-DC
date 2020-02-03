#ifndef CHECK_ITEMS_C
#define CHECK_ITEMS_C

#include "DBdefs.h"
#include "cl4.h"
//#define _LARGEFILE64_SOURCE

void _chkitems(NODE *NodeP, short RequiredEntries)
{
	NODE_1	*N1p;
	NODE_2	*N2p;
	short	NewRecsInPage;

	//printf("_chkitems (NODE*: %08X, Required Entries: %04X\n",NodeP,RequiredEntries);

	if ( NodeP->RecsInPage < RequiredEntries + NodeP->NumEntries ) // do we need more space than currently allocated?
	{
		NewRecsInPage = (short)(NodeP->RecsInPage + RequiredEntries + 2);
		
		N1p = (NODE_1 *)ealloc(sizeof(NODE_1) * NewRecsInPage, 0);
		cpybuf((char *)N1p, (char *)NodeP->NODE1ptr, sizeof(NODE_1) * NodeP->RecsInPage);
		nfree(NodeP->NODE1ptr, 0);
		NodeP->NODE1ptr = N1p;
		
		N2p = (NODE_2 *)ealloc(sizeof(NODE_2) * (NewRecsInPage + 1), 0);
		cpybuf((char *)N2p, (char *)NodeP->NODE2ptr, sizeof(NODE_2) * (NodeP->RecsInPage + 1));
		nfree(NodeP->NODE2ptr, 0);
		NodeP->NODE2ptr = N2p;
		
		NodeP->RecsInPage = NewRecsInPage;
	}
}

#endif
