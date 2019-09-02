#ifndef LOCK_GENERAL_C
#define LOCK_GENERAL_C

#include "DBdefs.h"
#include "cl4.h"

int _lock(DBinfo *DBptr, short IndexOff, short LockMode)
{
	signed int v3; // edx@1
	signed int v4; // edi@2
	int v5; // edx@2

	//printf("_lock(DBptr : x%08X,IndexOff: x%04X, LockMode: %d)\n" ,DBptr, IndexOff, LockMode);

	v3 = 1;
	if ( (DBptr->DBopenflg & 0xFF) != 0xFF )
	{
		v4 = 0;
		v5 = lseek64(DBptr->DBfilehandle, (IndexOff + DBptr->DBmaxpages * DBptr->DBpgsize), 0);		// Actually Locks an area outside the file!
		if ( v5 >= 0 && lockf64(DBptr->DBfilehandle, lmap[LockMode], 1) >= 0 )					// Lock a 1 byte area for each page
			v4 = 1;
		v3 = v4;
	}
	return v3;
}


int _lockmap(DBinfo *DBptr, short LockMode)
{
	return _lock(DBptr, DBptr->DBmaxtables, LockMode);
}

int _lockpg(DBinfo *DBptr, int PageNo, int a3)
{
	return 1;
}

int _locktbl(DBinfo *DBptr, short IndexOff, int Lock)
{
	return 1;
}
#endif



