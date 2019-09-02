#ifndef CHKPAGE_C
#define CHKPAGE_C

#include "DBdefs.h"
#include "cl4.h"

bool _chkpage(TDinfo *TDptr, PAGE *PageBuffer)
{
	DBinfo	*DBptr;
	short	*v2;
	short	*v4;
	short	v7;
	short	v9 = 0;

	//printf("_chkpage(TDptr : x%08X, PageBuffer : x%08X, PageType = x%02X)\n",TDptr, PageBuffer, (unsigned char)PageBuffer->PageType);

	DBptr = TDptr->TDDBinfo;
	if ( PageBuffer->PageType & 0x40)
	{
		
		v4 = (short*)PageBuffer->TabStart_or_RecSize;
		v2 = v4 + PageBuffer->NumEntries;

		if ((char*)(&PageBuffer->PageType + DBptr->DBpgsize) <= (char*)v2 )				// Check that we don't exceed the page boundary
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
			if ( PageBuffer->PageType >= 0 )
				v9 = TDptr->TDRecSize * PageBuffer->NumEntries;
			else
				v9 = TDptr->TDKeySize * PageBuffer->NumEntries;
		}
		else	// V3,4 dbases
			v9 = (short)(int)PageBuffer->TabStart_or_RecSize * PageBuffer->NumEntries;
	}

	return &PageBuffer->TabEnd[v9] <= &PageBuffer->PageType + DBptr->DBpgsize;	// Check that we don't exceed the page boundary
	// return true == success, false == failure (page sanity checks failed)

}


#endif
