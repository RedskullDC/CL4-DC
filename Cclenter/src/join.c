#ifndef JOIN_C
#define JOIN_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

// join used both by create temp table routines, and join/range functions

short join(unsigned int TDno, int *TTnos, short NumTTs, EXPR *Select)
{
	char	*Buffer;
    short	count;
	short	TTidx;
	short	v11;
	short	result;
    short	BuffSize;

	char	*WorkAreas[8];	// only really need 6
    short	RowSizes[8];	// only really need 6  [0..5]

    if ( NumTTs > 6 )
        NumTTs = 6;

    for ( count = 0 ; count < NumTTs ; count++ )
    {
		RowSizes[count]	 = _rowsize(tblrow(TTnos[count]));
        WorkAreas[count] = (char *)alloc(RowSizes[count], 0);
        //fill(WorkAreas[count], RowSizes[count], 0);		// Not necessary. alloc fills buffer with ZEROs anyway.
    }

    BuffSize = _rowsize(tblrow(TDno));		// Setup dest TT buffer
    Buffer	 = (char *)alloc(BuffSize, 0);
    cdbrestrict(TTnos[0], Select);

    while ( 1 )
    {
        result = getnxt(TTnos[0], WorkAreas[0], Select);
        if ( result <= 0 )					// < 0 == ERROR, 0 == No more data
            break;
        
		fill(Buffer, BuffSize, 0);			// ensure buffer clear before copy
		TTidx = 1;
        while ( TTidx < NumTTs )
        {
			for ( count = 0; count < TTidx && samekey(TTnos[TTidx], WorkAreas[TTidx], TTnos[count], WorkAreas[count]); count++ )
				;

			if ( count != TTidx )
            {
				fill(WorkAreas[TTidx], RowSizes[TTidx], 0);

                for ( v11 = 0 ; v11 < TTidx ; v11++ )
					fldxfer(TTnos[TTidx], WorkAreas[TTidx], TTnos[v11], WorkAreas[v11]);

				getr(TTnos[TTidx], WorkAreas[TTidx], 0);
			}
            fldxfer(TDno, Buffer, TTnos[TTidx], WorkAreas[TTidx]);
			TTidx++;
        }

        fldxfer(TDno, Buffer, TTnos[0], WorkAreas[0]);			// Ensure precedence of key fields from TTnos[]	[0 > 1 > 2 > 3 ....] 

		if ( !Select || testr(TDno, Buffer, 0, Select) )		// Select == 0 means match all, testr() checks for a keymatch
        {
            result = loadr(TDno, Buffer, 0);					// load record to temp table , POS = 0 use default data layout
            if ( result < 0 )									// ERROR, couldn't load data for some reason
                break;
        }
    }

	for ( count = 0; count < NumTTs ; count++ )
		nfree(WorkAreas[count], 0);

    nfree(Buffer, 0);
    return result;				// 0 == success (all available data loaded), < 0 == ERROR
}

#endif
