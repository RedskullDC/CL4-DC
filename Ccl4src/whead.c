#ifndef WRITEHEAD_C
#define WRITEHEAD_C

#include "DBdefs.h"
#include "cl4.h"
//#define _LARGEFILE64_SOURCE
//void DumpPageTable(HDRTABLE* Table);

// write header info for a table into main index.
// 6 bytes, contains pointer to top page, and size of tree structure (Max Depth == 6)
// 6 bytes are free, presumably for future expansion.

void _whead(DBinfo *DBptr, short IndexOff, int PageNo, short Depth)
{
	HDRTABLE v5;

    //printf("_whead(DBptr: x%08X,IndexOff: x%04X, PageNo: %d, Depth : %d)\n" ,DBptr,IndexOff, PageNo, Depth);

	ltoms(&v5.PageNo, PageNo);
	itoms(&v5.Depth, Depth);
	
	if ( lseek64(DBptr->DBfilehandle, 12 * IndexOff + DBptr->DBtableaddress, 0) < 0 || write(DBptr->DBfilehandle, &v5, 6u) != 6 )
		derror(27, DBptr, 0);
}

#endif
