#ifndef GETNODE_C
#define GETNODE_C

#include "DBdefs.h"
#include "cl4.h"

NODE*	getnode(TDinfo *TDptr, int PageNo, char NodeType)
{
	PAGE	*PageBuff;
	NODE_1	*N1ptr;
	NODE_1	*v15;
	NODE	*NodePtr;
	short	*v4;
	size_t	DataSize;

	short	v6;
	unsigned short v9;
	short	Index;
	short	v16;
	short	PageType;


    //printf("getnode(TDptr: %08X, PageNo: x%04X [%4d] ,Type: %02X)\n",TDptr, PageNo,PageNo, NodeType);

	// indexpg and datapg have their own sanity checks on NodeType

	if ( NodeType & 2 )
		PageBuff = _indexpg(TDptr, PageNo);
	else
		PageBuff = _datapg(TDptr, PageNo);

	// Next block calculates the size of each record, and sets the N1ptr->Size value for each one.

	v15 = (NODE_1 *)ealloc(8 * (PageBuff->NumEntries + 2), 0);
	DataSize = 0;
	v16		 = 0;
	PageType = 0;

	if ( PageBuff->PageType & 0x40 )            // 0x40 is an indexed data page
	{
		v16 = 2;
		v4 = (short *)PageBuff->TabStart_or_RecSize;	// Pointer to TableStart in this case.
		for ( N1ptr = v15; N1ptr < &v15[PageBuff->NumEntries]; N1ptr++ )
		{
			v6 = ((short)SWAP_SHORT(v4)) - DataSize;
			N1ptr->Size = v6;                     // Go through the index table, calculate data size for each entry
			DataSize += v6;
			v4++;
		}
		PageType = 0x01;	//flagbits
	}
	else            // non-indexed data page? fixed record size
	{
		Index = PageBuff->NumEntries - 1;
		for ( N1ptr = &v15[Index]; (signed short)Index >= 0; --N1ptr )
		{
			v9 = (unsigned short)(int)PageBuff->TabStart_or_RecSize;	// RecSize in this case.
			N1ptr->Size = v9;
			DataSize += v9;
			Index--;
		}
	}
	
	//------------

	NodePtr = (NODE *)ealloc(DataSize + 40, 0); // sizeof(NODE) == 40. Tack data on the end.
	NodePtr->PageNo		= PageNo;
	NodePtr->NumEntries = PageBuff->NumEntries;
	NodePtr->DataEnd	= &PageBuff->TabEnd[DataSize - 16] - (char *)PageBuff;
	NodePtr->RecsInPage = PageBuff->NumEntries + 2;
	cpybuf(&NodePtr->DataStart, PageBuff->TabEnd, DataSize);// Copy page data to end of NODE structure

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

	NodePtr->NODE2ptr = (NODE_2 *)ealloc(4 * (NodePtr->RecsInPage + 1), 0);

	if ( PageBuff->PageType >= 0 )   // PageType 0x00 or 0x40
	{
		fill((char *)NodePtr->NODE2ptr, 4 * (NodePtr->RecsInPage + 1), 0);	// ealloc above zeroes the memory anyway...
		NodePtr->field_8 = 0;
		NodePtr->field_C = 0;
	}
	else      // PageType 0x80: Index of Pages No.s that belong to the Table
	{
		v16 += 4;
		NodePtr->field_8 = mstol((int *)&PageBuff->field_12);// ???  Next/Prev pages perhaps?
		NodePtr->field_C = mstol((int *)&PageBuff->field_16);// ??? Appear unused in latest version
		//printf("getnode - field_8 = %d, field_C = %d\n",NodePtr->field_8,NodePtr->field_C );
		
		for (Index = 0; PageBuff->NumEntries >= Index ; Index++ )
			NodePtr->NODE2ptr[Index].PageNo = mstol((int *)&PageBuff->DataStart[4 * Index]);

		PageType |= 0x0002u;
	}
	
	NodePtr->field_16 = v16;
	NodePtr->PageType = PageType;    // Bits 1,0 correspond to bits 7,6 on disk. : 0x03 -> 0xC0 for example
	return NodePtr;
}

//---------------


#endif