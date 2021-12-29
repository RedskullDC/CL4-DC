#ifndef DATAPG_C
#define DATAPG_C

#include "DBdefs.h"
#include "cl4.h"

/*
void* _datapg(TDinfo *TDptr, unsigned int PageNo)
{
	DBinfo *DBptr;
	char	*PageBuff;
	PAGE	*pg;
	unsigned char PageType;
	unsigned short NumEntries;
	char *v6;
	void *v7;

	//printf("_datapg(TDptr = x%08X, PageNo = %d)\n" ,TDptr,PageNo);
	//printf("DBfile %s offset = %d x %d = x%0X\n",TDptr->TDDBinfo->DBname, PageNo, TDptr->TDDBinfo->DBpgsize, PageNo*TDptr->TDDBinfo->DBpgsize);
	DBptr = TDptr->TDDBinfo;                   // Controlling DBase 
	if ( DBptr->DBmaxpages <= PageNo )           
		derror(44, 0, TDptr);
	PageBuff = _getpg(DBptr);           // Pass DBptr to calculate PageSize
	pg = (PAGE *) PageBuff;
	//printf("PageBuff    = x%08X \n",PageBuff);
	if ( !_readpg(DBptr,&PageBuff[16], PageNo) )             // 16byte header
		derror(46, 0, TDptr);
	
	PageType = PageBuff[16];
	//printf("PageType    = x%2X \n",PageType);
	if ( (char)PageType < 0 )				// exclude page types with 80 set
		derror(39, 0, TDptr);
	NumEntries = SWAP_SHORT(&PageBuff[16]) & 0x3FFF;
	//printf("NumEntries  = x%04X (%3d) \n",NumEntries,NumEntries);
	pg->NumEntries = NumEntries;

	if ( PageType & 0x40 )	// Indexed page with record offsets at the start
	{						// *variable* record size (contains string variables)
		v7 = &PageBuff[18];
		//printf("Index Table = x%08X\n",v7);
		v6 = v7 + 2 * NumEntries;
		//printf("Data ends   = x%08X\n",v6);
		pg->TabStart_or_RecSize = v7;	//&PageBuff[18];
	}
	else if ( DBptr->DBvflag == 2 )          // DBversionFlag
	{
		pg->TabStart_or_RecSize = (void*)TDptr->TDRecSize;		// fixed record size
		v6 = &PageBuff[18];
	}
	else	// Version 3 or 4 DBases, *fixed* data size. (contains no strings)
	{
		v6 = &PageBuff[20];
		//printf("Data Start: = %X \n",v6);
		v7 = (void*)SWAP_SHORT(&PageBuff[18]);
		//printf("Record Size = %X \n",v7);  // 
		pg->TabStart_or_RecSize = v7;
	}
	pg->TabEnd = v6;
	
	if ( !_chkpage(TDptr, pg) )
		derror(50, 0, TDptr);
	return PageBuff;
}
*/

PAGE* _datapg(TDinfo *TDptr, unsigned int PageNo)
{
	DBinfo *DBptr;
	PAGE	*pg;
	char PageType;
	unsigned short NumEntries;

	//printf("_datapg(TDptr = x%08X, PageNo = %d)\n" ,TDptr,PageNo);

	DBptr = TDptr->TDDBinfo;
	if ( DBptr->DBmaxpages <= PageNo )           
		derror(44, 0, TDptr);

	pg = (PAGE *) _getpg(DBptr);           // Pass DBptr to calculate PageSize
	
	if ( !_readpg(DBptr,(char*)&pg->pgData, PageNo) )             // 16byte header
		derror(46, 0, TDptr);
	
	NumEntries = __builtin_bswap16(pg->pgData.PageType) & 0x3FFF;
	PageType = (char) (__builtin_bswap16(pg->pgData.PageType) >> 8);

	if ( PageType < 0 )				// exclude page types with 80 set
		derror(39, 0, TDptr);
	
	pg->header.NumEntries = NumEntries;

	if ( PageType & 0x40 )					// Indexed page with record offsets at the start
	{										// *variable* record size (contains string variables)
		pg->header.tsrs.TabStart = (char*)&pg->pgData.field_12;
		pg->header.TabEnd = (char *)((short *)&pg->pgData.field_12 + NumEntries);
	}
	else if ( DBptr->DBvflag == 2 ) 
	{
		pg->header.tsrs.RecSize = TDptr->TDRecSize;
		pg->header.TabEnd = (char*)&pg->pgData.field_12;
  	}
	else	// Version 3 or 4 DBases, *fixed* data size. (contains no strings)
	{
		pg->header.tsrs.RecSize = (int) __builtin_bswap16(pg->pgData.field_12);
		pg->header.TabEnd = &pg->pgData.field_14;
 	}
	
	if ( !_chkpage(TDptr, pg) )
		derror(50, 0, TDptr);
	return pg;
}


#endif
