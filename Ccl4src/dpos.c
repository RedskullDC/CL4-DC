#ifndef DPOS_C
#define DPOS_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"

// descending pos   [see also _ipos()]

void _dpos(DPOS *dpos, int PageNo, short Depth, TDinfo *TDptr)
{
	short v5;
	PAGE *page;

	dpos->field_8	 = 0;
	dpos->NumEntries = 0;
	
	v5 = Depth - 1;		// How many branches does the tree descend into...
	while ( v5 > 0 )
	{
		page = _indexpg(TDptr, PageNo);
		dpos->field_8 = PageNo;				// Containing page

		dpos->NumEntries = page->header.NumEntries;
		PageNo = mstol((int*)&page->header.DataStart[4 * dpos->NumEntries]);	// Set to last entry in page
		--v5;
	}
	
	dpos->PageNo  = PageNo;
	dpos->field_4 = 0;
}

#endif
