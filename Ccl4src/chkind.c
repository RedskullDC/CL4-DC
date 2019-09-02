#ifndef CHKIND_C
#define CHKIND_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"                      // for Table index numbers

TDef* _chkindex(short TableIdx, DBinfo *DBptr)
{
	TDef *tdef;
	
	if ( TableIdx <= 2 && (DBptr->DBvflag > 3 || TableIdx != 2) )	// Only V4 DBases have locks table?
	{
		if ( DBptr->DBvflag == 2 )	// If this is a version #2 database
		{
			if ( TableIdx == 1 )	// remap "_fshape" references to
				TableIdx = 3;		// "_xfshape"
		}
		tdef = (TDef *)ealloc(sz_shape[TableIdx], 0);
		cpybuf((char *)tdef, (char *)def_0[TableIdx], sz_shape[TableIdx]);
		return tdef;
	}
	else
		return 0;	// tdef = 0;
}

// #define	td_tables		0	// "tables"
// #define	td_fields		1	// "fields"
// #define	td_locks		2	// "locks"

#endif
