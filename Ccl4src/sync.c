#ifndef SYNC_C
#define SYNC_C

#include "DBdefs.h"
#include "cl4.h"
//#define _LARGEFILE64_SOURCE

int emark(DBinfo *DBptr, short IndexOff)
{
	int result; // eax@1

    //printf("emark(DBptr: x%08X, IndexOff: x%04X)\n" ,DBptr, IndexOff);

	result = _lock(DBptr, IndexOff, 1);
	if ( !result )
		derror(18, DBptr, 0);
	return result;
}

int ewake(DBinfo *DBptr, short IndexOff)
{
	//printf("ewake(DBptr: x%08X, IndexOff: x%04X)\n" ,DBptr, IndexOff);

	return _lock(DBptr, IndexOff, 0);
}

void ewait(DBinfo *DBptr, short IndexOff)
{
	// do nothing in linux
	;
}

#endif
