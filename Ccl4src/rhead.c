#ifndef HEADER_C
#define HEADER_C

#include "DBdefs.h"
#include "cl4.h"
//#define _LARGEFILE64_SOURCE

short _rhead( DBinfo *DBptr, short IndexOffset, int *PageNo)
{
	register short Depth;
	HDRTABLE buf;

    //printf("_rhead(DBptr: x%08X,IndexOff: x%04X,PagePtr: x%08X)\n" ,DBptr,IndexOffset, PageNo);
	if ( DBptr->DBvflag != 2 && DBptr->DBmaxtables <= IndexOffset
		|| lseek64(DBptr->DBfilehandle, 12 * (signed int)IndexOffset + (off64_t)DBptr->DBtableaddress, 0) < 0
		|| readfull(DBptr->DBfilehandle, (char *)&buf, 6) != 6 )
		derror(26, DBptr, 0);
	
	//DumpPageTable(&buf);

	*PageNo = mstol(&buf.PageNo);
	Depth	= MSHORT(buf.Depth);
  
	if ( Depth > 6u )	// Table tree can only be 6 levels deep
		derror(20, DBptr, 0);
  
	return Depth;
}

#endif
