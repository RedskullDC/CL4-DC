#ifndef RELNODE_C
#define RELNODE_C

#include "DBdefs.h"
#include "cl4.h"

void relnode(NODE *Node)
{
    //printf("relnode(Node: x%08X)\n" ,Node);
	if ( Node )
	{
		nfree(Node->NODE1ptr, 0);
		nfree(Node->NODE2ptr, 0);
		nfree(Node, 0);
	}
}
#endif
