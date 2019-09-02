#ifndef LEHDTXT_C
#define LEHDTXT_C
#include "DBdefs.h"
#include "cl4.h"

short lehdtxt(void)
{
    XTAB	*xtab;
	short	XTno;
	short	result = 0;			// default to fail. 0 = error
	 
    XTno = getxtmem();
    allxtpt(XTno, &xtab);
    
	if ( getadwid(xtab) && getcol(xtab, col_HEAD) && getrow(xtab, row_HEAD) && getprmpt(xtab) )
		result = XTno;
	
	dallxtpt(&xtab);
    return result;
}

#endif
