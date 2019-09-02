#ifndef PUTNODE_C
#define PUTNODE_C

#include "DBdefs.h"
#include "cl4.h"

// Not public in libcl4:
char* sdata(char *Dest, NODE_1 *Node1ptr,short NumEntries)
{
	NODE_1 *v3; // esi@1
	short i; // di@1

	//printf("sdata(Dest : x%08X, Node1Ptr : x%08X, NumEntries :%d)\n",Dest,Node1ptr,NumEntries);
	v3 = Node1ptr;
	for ( i = NumEntries; i > 0; --i )
	{
		Dest += cpybuf(Dest, v3->Data, v3->Size);
		++v3;
	}
	return Dest;
}

// Not public in libcl4:
short* soffset(short *Dest, NODE_1 *Node1ptr, short NumEntries)
{
	NODE_1 *v3; // edi@1
	short v4; // si@1
	short i; // [sp+8h] [bp-10h]@1

	//printf("soffset(Dest : x%08X, Node1Ptr : x%08X, NumEntries :%d)\n",Dest,Node1ptr,NumEntries);

	v3 = Node1ptr;
	v4 = NumEntries;
	for ( i = 0; v4 > 0; ++v3 )
	{
		i += v3->Size;	// Fixed RecSize
		itoms(Dest, i);
		++Dest;
		--v4;
	}
	return Dest;
}

// Not public in libcl4:
char *rdata(char *Dest, NODE_1 *a2, short NumEntries)
{
    REFR *refr; // esi@1
    short idx; // di@1

	//printf("rdata(Dest : x%08X, REFR : x%08X, NumEntries :%d)\n",Dest,a2,NumEntries);
    refr = (REFR*)a2;	// a2 is actually a REFR* for TMP tables
    for ( idx = NumEntries; idx > 0; --idx )
    {
        //Dest += cpybuf(Dest, refr->Offset, *(_DWORD *)(*((_DWORD *)refr->Data - 1) + 4) - (unsigned int)refr->Offset);
		Dest += cpybuf(Dest, refr->Offset, ((*(refr->Data - 1))->Offset - refr->Offset));
        ++refr;
    }
    return Dest;
}

// Not public in libcl4:
short *roffset(short *a1, NODE_1 *a2, short NumEntries)
{
    REFR *refr; 
    short v4; 
    short idx;

	//printf("roffset(a1 : x%08X, REFR : x%08X, NumEntries :%d)\n",a1,a2,NumEntries);
    refr = (REFR*)a2;	// a2 is actually a REFR* for TMP tables
    v4 = NumEntries;
    for ( idx = 0; v4 > 0; ++refr )
    {
        //idx += *(_WORD *)(*((_DWORD *)refr->Data - 1) + 4) - LOWORD(refr->Offset);// idx += (*refr->Data[-1]->Offset) - refr->Offset
        idx += (short)((*(refr->Data - 1))->Offset - refr->Offset);

		itoms(a1, idx);
        ++a1;
        --v4;
    }
    return a1;
}


int* vectoms(int *a1, NODE_2 *Node2ptr, short NumEntries)
{
	short v3; // si@1
	NODE_2 *i; // edi@1

	v3 = NumEntries;
	for ( i = Node2ptr; v3 > 0; ++i )
	{
		ltoms(a1, i->PageNo);
		--v3;
		++a1;
	}	
	return a1;
}

//---------------

bool putnode(TDinfo *TDptr, NODE *Node)
{
	char *v2;
	unsigned short v3;
	short *v6;
	short RecSize;
	char *v11;
	bool result;

    //printf("putnode(TDptr: %08X, PageNo: x%04X [%4d] , PageType: %02X)\n",TDptr, Node->PageNo,Node->PageNo, Node->PageType & 0x0F);

	v2 = _getpg(TDptr->TDDBinfo) + 16;
	v3 = Node->NumEntries;
	if ( Node->PageType & 2 )
	{
		if ( Node->PageType & 1 )
			v3 |= 0xC000;				// Map PageType bits 0&1 -> 14&15
		else
			v3 |= 0x8000;

		itoms((unsigned short*)v2, v3);			// 2 bytes PageType written to disk
		ltoms((unsigned int*)(v2 + 2), Node->field_8);		//??
		ltoms((unsigned int*)(v2 + 6), Node->field_C);		//??
		v6 = (short*)vectoms((unsigned int*)(v2 + 10), Node->NODE2ptr, Node->NumEntries + 1);	//Normally a pagelist
		RecSize = TDptr->TDKeySize;				// RecSize is actually the size of "Key Values"
	}
	else
	{
		if ( Node->PageType & 1 )
			v3 |= 0x4000u;
		itoms((unsigned short*)v2, v3);			// PageType written to disk
		v6 = (unsigned short*)(v2 + 2);			// Start of data
		RecSize = TDptr->TDRecSize;
	}

	if ( Node->PageType & 1 )
	{
		if ( Node->PageType & 8 )
			v6 = roffset(v6, Node->NODE1ptr, Node->NumEntries);	// These are tables in a TEMP DBase
		else
			v6 = soffset(v6, Node->NODE1ptr, Node->NumEntries);
	}
	else
	{
		if ( TDptr->TDDBinfo->DBvflag != 2 )	// DBase version flag
		{
			itoms(v6, RecSize);					// 2 bytes of record size, then bump ptr
			++v6;
		}
	}
	
	// write the actual data to the page buffer
	if ( Node->PageType & 8 )
		v11 = rdata((char*)v6, Node->NODE1ptr, Node->NumEntries);	// These are tables in a TEMP DBase
	else
		v11 = sdata((char*)v6, Node->NODE1ptr, Node->NumEntries);

	// Fill remainder of buffer with $FF
	fill(v11, TDptr->TDDBinfo->DBpgsize - (v11 - v2), 0xFF);
	
	result = _writepg(TDptr->TDDBinfo, v2, Node->PageNo);
	if ( !result )
		derror(11, 0, TDptr);
	return result;
}
#endif
