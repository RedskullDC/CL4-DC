#ifndef INDEX_POS_C
#define INDEX_POS_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

// increasing pos   [see also _dpos()]

void _ipos(DPOS *dpos, int PageNo, short Depth, TDinfo *TDptr)
{
	short v5;
	PAGE *page;

//printf("_ipos - PageNo = %d\n",PageNo);

	dpos->field_8	 = 0;
	dpos->NumEntries = 0;
	
	v5 = Depth - 1;
	while ( v5 > 0 )
	{
		page = _indexpg(TDptr, PageNo);
		dpos->field_8 = PageNo;					// Containing Page
		
		PageNo = mstol((int*)page->header.DataStart);	// Set to first entry in page
		dpos->NumEntries = 1;
		--v5;
	}
	
	dpos->PageNo  = PageNo;
	dpos->field_4 = 0;
}


#endif
