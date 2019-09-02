#ifndef FRESHNODE_C
#define FRESHNODE_C

#include "DBdefs.h"
#include "cl4.h"

NODE* freshnode(TDinfo *TDptr, short NodeType)
{
	NODE	*Node;
	int		PageNo;
	int		v5;
	
	//printf("freshnode(TDptr: %08X, TDptr->TDFlags : x%04X, NodeType: x%04X )\n",TDptr, TDptr->TDFlags, NodeType);
	PageNo = _capture(TDptr->TDDBinfo);	// Go get a free Page.
	if ( !PageNo )
		derror(42, 0, TDptr);
	
	Node = ealloc(sizeof(NODE), 0);		// 40
	Node->PageNo	 = PageNo;
	Node->NumEntries = 0;
	Node->field_8	 = 0;
	Node->field_C	 = 0;
	
	if ( NodeType & 2 )
	{
		Node->DataEnd = 14;
		if ( (char)TDptr->TDFlags >= 0 )
		{
			if ( TDptr->TDDBinfo->DBvflag != 2 )	// DB version
				Node->DataEnd += 2;
			Node->PageType = 0x02;
			Node->field_16 = 8;
			v5 = TDptr->TDKeySize;
		}
		else
		{
			Node->PageType = 0x03;
			Node->field_16 = 8;
			v5 = 1;
		}
	}
	else
	{
		Node->DataEnd = 2;
		if ( TDptr->TDFlags & 0x40 )
		{
			Node->PageType = 0x01;
			Node->field_16 = 2;
			v5 = TDptr->TDRecMin;
		}
		else
		{
			if ( TDptr->TDDBinfo->DBvflag != 2 )	// DB version
				Node->DataEnd += 2;
			Node->PageType = 0;
			Node->field_16 = 0;
			v5 = TDptr->TDRecSize;
		}
	}	
	Node->RecsInPage = (TDptr->TDDBinfo->DBpgsize - Node->DataEnd) / (v5 + Node->field_16);
	Node->NODE1ptr = (NODE_1*)ealloc(sizeof(NODE_1) * Node->RecsInPage, 0);
	Node->NODE2ptr = (NODE_2*)ealloc(sizeof(NODE_2) * (Node->RecsInPage + 1), 0);
	return Node;
}

#endif
