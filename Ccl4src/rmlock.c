#ifndef RMLOCK_C
#define RMLOCK_C

#include "DBdefs.h"
#include "cl4.h"

void _rmlock(DBinfo *DBptr, int *PageList)
{
	int *PageNo; // esi@1

    //printf("_rmlock(DBptr: x%08X, PageList: x%08X)\n" ,DBptr, PageList);
	for ( PageNo = PageList; *PageNo; PageNo++ )
		_lockpg(DBptr, *PageNo, 0);
}


#endif
