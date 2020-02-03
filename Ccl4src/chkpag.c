#ifndef CHKPAGE_C
#define CHKPAGE_C

#include "DBdefs.h"
#include "cl4.h"

/*
bool _chkpage(TDinfo *TDptr, PAGE *pg)
{
	DBinfo	*DBptr;
	short	*v2;
	short	*v4;
	short	v7;
	short	v9 = 0;

	//printf("_chkpage(TDptr : x%08X, pg : x%08X, PageType = x%02X)\n",TDptr, pg, (unsigned char)pg->PageType);

	DBptr = TDptr->TDDBinfo;
	if ( pg->PageType & 0x40)
	{
		
		v4 = (short*)pg->TabStart_or_RecSize;
		v2 = v4 + pg->NumEntries;

		if ((char*)(&pg->PageType + DBptr->DBpgsize) <= (char*)v2 )				// Check that we don't exceed the page boundary
			return false;

		while ( v4 < v2 )
		{
			v7 = SWAP_SHORT(v4);
			if ( v7 <= v9 )			// index error Index value was less than previous value!
				return false;
			v9 = v7;
			v4++;
		}
	}
	else
	{
		if ( DBptr->DBvflag == 2 )	// old style dbases
		{
			if ( pg->PageType >= 0 )
				v9 = TDptr->TDRecSize * pg->NumEntries;
			else
				v9 = TDptr->TDKeySize * pg->NumEntries;
		}
		else	// V3,4 dbases
			v9 = (short)(int)pg->TabStart_or_RecSize * pg->NumEntries;
	}

	return &pg->TabEnd[v9] <= &pg->PageType + DBptr->DBpgsize;	// Check that we don't exceed the page boundary
	// return true == success, false == failure (page sanity checks failed)

}
*/
bool _chkpage(TDinfo *TDptr, PAGE *pg)
{
	DBinfo	*DBptr;
	short	*TBL_LAST;
	short	*TBL_CURRENT;
	short	v7;
	short	v9 = 0;
	char	PageType;

	DBptr = TDptr->TDDBinfo;
	PageType = (char)(__builtin_bswap16(pg->pgData.PageType) >> 8);

	if ( PageType & 0x40)
	{
		TBL_CURRENT = (short *)pg->header.tsrs.TabStart;
		TBL_LAST = &TBL_CURRENT[pg->header.NumEntries];

		if ((char*)&pg->pgData.PageType + DBptr->DBpgsize <= (char*)TBL_LAST )				// Check that we don't exceed the page boundary
			return false;
		
		v9 = __builtin_bswap16(*TBL_CURRENT);
		if (v9 <= 0)
			return false;
		TBL_CURRENT++;

		while ( TBL_CURRENT < TBL_LAST )
		{
			v7 = __builtin_bswap16(*TBL_CURRENT);
			if ( v7 <= v9 )								// index error! Current Index value was less than previous value!
				return false;
			v9 = v7;
			TBL_CURRENT++;
		}
	}
	else
	{
		if ( DBptr->DBvflag == 2 )	// old style dbases
		{
			if ( PageType >= 0 )
				v9 = TDptr->TDRecSize * pg->header.NumEntries;
			else
				v9 = TDptr->TDKeySize * pg->header.NumEntries;
		}
		else	// V3,4 dbases
			v9 = pg->header.NumEntries * pg->header.tsrs.RecSize;
	}

	return (&pg->header.TabEnd[v9] <= (char*)&pg->pgData + DBptr->DBpgsize);	// Check that we don't exceed the page boundary
//***
	// return true == success, false == failure (page sanity checks failed)

}


#endif
