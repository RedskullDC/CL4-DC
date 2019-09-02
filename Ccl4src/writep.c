#ifndef WRITEPAGE_C
#define WRITEPAGE_C

#include "DBdefs.h"

bool _writepg(DBinfo *DBptr, char *Buffer, unsigned int PageNo)
{
	bool wrt_ok = false;

	//printf("_writepg(DBptr: x%08X,Buffer: x%08X,PageNo x%04X (%4d)\n" ,DBptr,Buffer,PageNo,PageNo);

	if ( lseek64(DBptr->DBfilehandle, PageNo * DBptr->DBpgsize, 0) >= 0 )
	{
		if ( write(DBptr->DBfilehandle, Buffer, DBptr->DBpgsize) == DBptr->DBpgsize )
			wrt_ok = true;
	}
	if ( !wrt_ok )
		printf("_writepg: num=[%d]\n", PageNo);
	return wrt_ok;
}
#endif
