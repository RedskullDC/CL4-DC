#ifndef GETNODE_C
#define GETNODE_C

#include "DBdefs.h"
#include "cl4.h"

NODE*	getnode(TDinfo *TDptr, int PageNo, char NodeType)
{
	PAGE	*pg;
	NODE_1	*N1ptr;
	NODE_1	*v15;
	NODE	*NodePtr;
	short	*v4;
	size_t	DataSize;

	short	v6;
	short	v9;
	short	Index;
	short	v16;
	short	PageType;


    //printf("getnode(TDptr: %08X, PageNo: x%04X [%4d] ,Type: %02X)\n",TDptr, PageNo,PageNo, NodeType);

	// indexpg and datapg have their own sanity checks on NodeType

	if ( NodeType & 2 )
		pg = _indexpg(TDptr, PageNo);
	else
		pg = _datapg(TDptr, PageNo);

	// Next block calculates the size of each record, and sets the N1ptr->Size value for each one.

	v15 = (NODE_1 *)ealloc(sizeof(NODE_1) * (pg->header.NumEntries + 2), 0);		// 8 on X86, 12 on X64?
	DataSize = 0;
	v16		 = 0;
	PageType = 0;

	if ( pg->pgData.PageType & 0x40 )            // (NOT BYTE SWAPPED) 0x40 is an indexed data page
	{
		v16 = 2;
		v4 = (short *)pg->header.tsrs.TabStart;
		for ( N1ptr = v15; N1ptr < &v15[pg->header.NumEntries]; N1ptr++ )
		{
			v6 = __builtin_bswap16(*v4) - DataSize;
			N1ptr->Size = v6;                     // Go through the index table, calculate data size for each entry
			DataSize += v6;
			v4++;
		}
		PageType = 0x01;	//flagbits
	}
	else            // non-indexed data page? fixed record size
	{
		Index = pg->header.NumEntries - 1;
		for ( N1ptr = &v15[Index]; Index >= 0; --N1ptr )
		{
			v9 = pg->header.tsrs.RecSize;
			N1ptr->Size = v9;
			DataSize += v9;
			Index--;
		}
	}
	
	//------------

	NodePtr = (NODE *)ealloc(DataSize + sizeof(NODE), 0); // sizeof(NODE) == 40. Tack data on the end.

	NodePtr->PageNo		= PageNo;
	NodePtr->NumEntries = pg->header.NumEntries;
	NodePtr->DataEnd	= &pg->header.TabEnd[DataSize - sizeof(PAGEHDR)] - (char *)pg;
	NodePtr->RecsInPage = pg->header.NumEntries + 2;
	cpybuf(&NodePtr->DataStart, pg->header.TabEnd, DataSize);// Copy page data to end of NODE structure

	// Go through and set the N1ptr->Data pointers into the copied NODE data buffer

	Index = NodePtr->NumEntries - 1;		// Last N1ptr is a NULL structure
	for ( N1ptr = &v15[Index]; Index >= 0; --N1ptr )
	{
		DataSize -= N1ptr->Size;
		N1ptr->Data = &NodePtr->DataStart + DataSize;// N1ptrs index into the Node buffer area.
		Index--;
	}
	NodePtr->NODE1ptr = v15;		// Start of N1p array
	
	//--------
	// NODE_2 records are a list of PAGE numbers

	NodePtr->NODE2ptr = (NODE_2 *)ealloc(sizeof(NODE_2) * (NodePtr->RecsInPage + 1), 0);

	if ( pg->pgData.PageType & 0x80 )   // PageType 0x80: Index of Pages No.s that belong to the Table
	{
		v16 += 4;
		NodePtr->field_8 = mstol((int *)&pg->pgData.field_12);// ???  Next/Prev pages perhaps?
		NodePtr->field_C = mstol((int *)&pg->pgData.field_16);// ??? Appear unused in latest version
		//printf("getnode - field_8 = %d, field_C = %d\n",NodePtr->field_8,NodePtr->field_C );
		
		for (Index = 0; pg->header.NumEntries >= Index ; Index++ )
			NodePtr->NODE2ptr[Index].PageNo = mstol((int *)&pg->header.DataStart[4 * Index]);

		PageType |= 0x0002u;
	}
	else  // PageType 0x00 or 0x40
	{
		fill((char *)NodePtr->NODE2ptr, sizeof(NODE_2) * (NodePtr->RecsInPage + 1), 0);	// ealloc above zeroes the memory anyway...
		NodePtr->field_8 = 0;
		NodePtr->field_C = 0;
	}
	
	NodePtr->field_16 = v16;
	NodePtr->PageType = PageType;    // Bits 1,0 correspond to bits 7,6 on disk. : 0x03 -> 0xC0 for example
	return NodePtr;
}

//---------------


#endif
