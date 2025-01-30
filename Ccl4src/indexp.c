#ifndef INDEXP_C
#define INDEXP_C

#include "DBdefs.h"
#include "cl4.h"
void DumpIndexPage(char* Buffer);

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

	//DumpIndexPage(PageBuff);
	return pg;
}


#endif
