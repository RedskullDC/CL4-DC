#ifndef INDEXP_C
#define INDEXP_C

#include "DBdefs.h"
#include "cl4.h"
void DumpIndexPage(char* Buffer);

/*
PAGE* _indexpg(TDinfo *TDptr, unsigned int PageNo)
{
	char	*PageBuff;
	PAGE	*pg;
	char	*v6;
	char	*v7;
	DBinfo	*DBptr;
	char	*v14;
	unsigned char PageType; // al@3
	unsigned short NumEntries; // ecx@5

	//printf("_indexpg(TDptr = x%08X, PageNo = %d)\n" ,TDptr,PageNo);
	//printf("DBfile %s offset = (x%04X) %d x %d = x%0X\n",TDptr->TDDBinfo->DBname,PageNo,PageNo, TDptr->TDDBinfo->DBpgsize, PageNo*TDptr->TDDBinfo->DBpgsize);
	DBptr = TDptr->TDDBinfo;                     
	if ( DBptr->DBmaxpages <= PageNo )		// Check range on PageNo
		derror(44, 0, TDptr);
	PageBuff = _getpg(DBptr);           // Get work area + 16bytes. Resizes if necessary
	pg = (PAGE *) PageBuff;
	if ( !_readpg(DBptr, &PageBuff[16], PageNo) )
		derror(46, 0, TDptr);
	PageType = PageBuff[16];                               // Start of pagedata
	//printf("PageType   = x%02X \n",PageType);
	if ( (char)PageType >= 0 )				// High bit denotes index page!
		derror(13, 0, TDptr);
	NumEntries = SWAP_SHORT(&PageBuff[16]) & 0x3FFF;
	//printf("NumEntries = x%04X (%3d) \n",NumEntries+1 ,NumEntries+1);
	pg->NumEntries = NumEntries;
  
	v6 = &PageBuff[26];
	//printf("DataStart  = x%08X \n",v6);
	pg->DataStart = v6;

	if ( PageType & 0x40 )
	{
		//printf("PageType&40\n");
		//v13 = NumEntries;
		v14 = &v6[4 * (NumEntries + 1)];
		//printf("KeySizePtr = x%08X (KeySize = %d) \n",v14,SWAP_SHORT(v14));
		v7 = v14 + 2 * NumEntries;
		//printf("KeyDataPtr = x%08X \n",v7);
		pg->TabStart_or_RecSize = v14;  // ptr to (short) RecSize in this case
	}
	else if ( DBptr->DBvflag == 2 ) // Old style DBase
	{
		//printf("vflag == 2\n");
		//*((int *)PageBuff + 2) = TDptr->TDIrecsize ;
		pg->TabStart_or_RecSize = (void*)TDptr->TDRecSize;
		//printf("TDRecSsize = %X \n",TDptr->TDRecSize);
		v7 = &v6[4 * (NumEntries + 1)];
		//printf("v7 = %X \n",v7);
	}
	else	// Newer style DBase ( VFLAG = 3,4 )
	{
		v7 = &v6[4 * NumEntries + 6];
		*((int *)PageBuff + 2) = v6[4 * NumEntries + 5] | (v6[4 * NumEntries + 4] << 8);
		//pg->TabStart_or_RecSize = v6[4 * NumEntries + 5] | (v6[4 * NumEntries + 4] << 8);
	}
	pg->TabEnd = v7;

	if ( !_chkpage(TDptr, pg) )
		derror(51, 0, TDptr);

	//DumpIndexPage(PageBuff);
	return pg;
}
*/

PAGE* _indexpg(TDinfo *TDptr, unsigned int PageNo)
{
	PAGE	*pg;
	unsigned short *v6;
	DBinfo	*DBptr;
	TSRS	tsrs;
	char PageType; // al@3
	unsigned short NumEntries; // ecx@5
	int		v9;

	DBptr = TDptr->TDDBinfo;                     
	if ( DBptr->DBmaxpages <= PageNo )		// Check range on PageNo
		derror(44, 0, TDptr);
	
	pg = (PAGE *) _getpg(DBptr);           // Get work area + 16bytes, 32 on X64. Resizes if necessary
	
	if ( !_readpg(DBptr, (char*)&pg->pgData , PageNo) )
		derror(46, 0, TDptr);

	NumEntries = __builtin_bswap16(pg->pgData.PageType) & 0x3FFF;
	PageType = *(char*)&pg->pgData.PageType;

	if ( PageType >= 0 )				// High bit denotes index page!
		derror(13, 0, TDptr);

	pg->header.NumEntries = NumEntries;
  
	v6 = &pg->pgData.field_1A;
	
	pg->header.DataStart = (char*)&pg->pgData.field_1A;

	if ( PageType & 0x40 )
	{
		tsrs.TabStart = (char*)&v6[2 * (NumEntries + 1)];
	    pg->header.tsrs = tsrs;
	    pg->header.TabEnd = &tsrs.TabStart[2 * NumEntries];

	}
	else if ( DBptr->DBvflag == 2 ) // Old style DBase
	{
		pg->header.tsrs.RecSize = TDptr->TDRecSize;
		pg->header.TabEnd		= (char*)&v6[2 * (NumEntries + 1)];
	}
	else	// Newer style DBase ( VFLAG = 3,4 )
	{
		v9 = 2 * (NumEntries + 1);
		pg->header.tsrs.RecSize =  (int) __builtin_bswap16(v6[v9]);
		pg->header.TabEnd		= (char*)&v6[v9 + 1];
 	}

	if ( !_chkpage(TDptr, pg) )
		derror(51, 0, TDptr);

	return pg;
}


#endif
