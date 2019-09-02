#ifndef MODPAGE_C
#define MODPAGE_C

#include "DBdefs.h"
#include "cl4.h"

void modpage(NODE *Node, short N1_2idx, char *RecBuffer, short NewRecSize)
{
	NODE_1 *N1p;
	short SizeDiff;

	N1p = &Node->NODE1ptr[N1_2idx];
	N1p->Data = RecBuffer;
	SizeDiff = (NewRecSize - N1p->Size);	// calc difference in record size
	N1p->Size = NewRecSize;
	Node->DataEnd += SizeDiff;				// and adjust total as necessary
}

#endif

