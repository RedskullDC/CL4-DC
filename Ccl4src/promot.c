#ifndef PROMOTE_C
#define PROMOTE_C

#include <sys/param.h>                 // for MIN/MAX
#include "DBdefs.h"
#include "cl4.h"

short _promote(char *a1, NODE *Node, NODE *a3, short a4, TDinfo *TDptr)
{
	NODE_1 node1;
    //printf("_promote(a1: x%08X, Node: x%08X, Node2: x%08X, a4: x%04X, TDptr = x%08X)\n",a1,Node,a3,a4,TDptr);

	if ( Node->PageType & 2 )
	{
		cpybuf(&node1.Data, &Node->NODE1ptr[Node->NumEntries - 1].Data, 8u);	// sizeof(NODE_1) == 8
		delpage(Node, Node->NumEntries - 1);
	}
	else
	{
		node1.Data = a3->NODE1ptr[0].Data;

		if ( a4 )	// a4 =  ND_parent->PageType & 1
			node1.Size = _diffkey(Node->NODE1ptr[Node->NumEntries - 1].Data, node1.Data, TDptr->TableDefs);
		else
			node1.Size = (short)TDptr->TDKeySize;
	}

	return cpybuf(a1, node1.Data, MIN(node1.Size,TDptr->TDKeySize));		// return bytes copied == New record size
}

#endif
