#ifndef SCANPAGE_C
#define SCANPAGE_C

#include <sys/param.h>			// for MIN/MAX
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

short _cmp(PAGE_NODE p_pgnode, short N1_2idx, TDinfo *TDptr, short ArgType, short *a5)
{
	TDef	*tdef;
	char	*KeyDefs;
	char	*RecBuff;
	NODE	*NodePtr;
	PAGE	*PagePtr;

	short	TDFtype;
	short	v13;
	short	v14;
	unsigned short v12;
	unsigned short KeySize;
	unsigned short RecSize;
	unsigned short TDFsize2;

	//printf("_cmp(PAGEptr: x%08X, Index: x%02X (%4d),TDptr: x%08X, ArgType: x%02X,a5: x%08X) : " ,PageBuffer,N1_2idx,N1_2idx,TDptr,ArgType,a5);
	
	if ( ArgType & 8 )								// if (ArgType& 8), PagePtr is a PAGE structure, otherwise it is a NODE !!!
	{	
		RecBuff	= _itoptr(p_pgnode.PAGE, N1_2idx);            // PAGE here
		RecSize = _itosz(p_pgnode.PAGE, N1_2idx);
	}
	else
	{
		RecBuff	= p_pgnode.NODE->NODE1ptr[N1_2idx].Data;	// NODE structure here :   
		RecSize = p_pgnode.NODE->NODE1ptr[N1_2idx].Size; 
	}

	KeyDefs = TDptr->TDKeyDefs;
	KeySize = TDptr->KeyDefSize;
	
	tdef	= TDptr->TableDefs;						// Assume that pointer is valid
	TDFtype = tdef->TDFtype;
	*a5 = 0;										// update caller mem directly

	while ( tdef->TDFentry && (TDFtype & tdf_KEY) )	// &0x01 indicates a Key field
	{
		if ( TDFtype & tdf_STRING )		// string variable
			TDFsize2 = _fwidth(RecBuff, MIN(RecSize,tdef->TDFsize), TDFtype);
		else
			TDFsize2 = MIN(RecSize,tdef->TDFsize);
		
		if ( tdef->TDFtype & tdf_STRING ) // string variable
			v12 = _fwidth(KeyDefs, MIN(KeySize,tdef->TDFsize), TDFtype);
		else
			v12 = MIN(KeySize,tdef->TDFsize);

		//TDFsize2 = v9;
		
		//printf("_cmp: Before cmpattrib (RecBuff: x%08X, TDFsize2: x%02X, KeyDefs: x%08X, Width: x%02X, TDFtype: x%04X)\n",RecBuff, TDFsize2, KeyDefs, v12, TDFtype);
		//DumpBlock(RecBuff,TDFsize2);
		//DumpBlock(KeyDefs,v12);

		v13 = _cmpattrib(RecBuff, TDFsize2, KeyDefs, v12, TDFtype);
		//printf("_cmp: returned %d\n",v13);
		
		if ( v13 )
			return v13;			// didn't match, return offset in # recs?
		
		v14 = TDFsize2 - v12;
		*a5 = v14;				//TDFsize2 - v12;		// update caller mem directly
		if ( TDFsize2 != v12 )
		{
			if ( v14 > 0 )
				v14 = 0;
			//printf("returning v14 %d\n",v14);
			return v14;
		}

		RecBuff	+= TDFsize2;
		RecSize -= TDFsize2;
		KeyDefs += v12;
		KeySize -= v12;
		
		tdef++;
		TDFtype = tdef->TDFtype;
	}
	return 0;	// exact match, or tdef was empty
}

bool _scanpg(PAGE_NODE p_pgnode, TDinfo *TDptr, short *N2idx, short ArgType)
{
	short	IndexLo;
	short	IndexHi;
	short	IndexTest; 
	short	v6;
	bool	v7; 
	short	NumEntries;
	short	a5;


	//printf("_scanpg(p_pgnode: x%08X, TDptr: x%08X, N2idxPtr: x%08X, ArgType: x%02X)\n" ,p_pgnode, TDptr,N2idx,ArgType);

	if ( ArgType & 8 )                      // if (ArgType& 8), PagePtr is a PAGE structure, otherwise it is a NODE
		NumEntries = p_pgnode.PAGE->header.NumEntries;	// PagePtr was a PAGE here. get PagePtr->NumEntries
	 else
		NumEntries = p_pgnode.NODE->NumEntries;	// PagePtr was a NODE here. get PagePtr->NumEntries
	
	// Quick search routine.
	// start in middle (IndexHi+a2)/2, then keep halving till we hit desired record
	IndexLo = 0;
	IndexHi = NumEntries - 1;
	while ( IndexLo <= IndexHi )
	{
		IndexTest = (IndexHi + IndexLo) / 2;
		//printf("IndexLo = %3d, IndexHi = %3d , IndexTest = %3d\n",IndexLo, IndexHi, IndexTest);
		v6 = _cmp(p_pgnode, IndexTest, TDptr, ArgType, &a5);
		if ( v6 < 0 || v6 <= 0 && ArgType & 2 )
			IndexLo = IndexTest + 1;
		else
			IndexHi = IndexTest - 1;
	}
	//printf("_scanpg: returned *N2idx  = %d \n",IndexLo);
	*N2idx = IndexLo;	// update caller value
	
	//-----------
	
	v7 = false;
	if ( !(ArgType & 2) )
	{
		if ( IndexLo < NumEntries )
		{
			if ( !_cmp(p_pgnode, IndexLo, TDptr, ArgType, &a5) && a5 <= 0 )
				v7 = true;
		}
	}
	//printf("_scanpg: returned - %s\n",v7 ? "true":"false");
	return v7;
}
#endif

