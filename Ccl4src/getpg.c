#ifndef GETPG_C
#define GETPG_C

#include "DBdefs.h"

void cdbfreepg(void)
{
	if ( pbuf )
	{
		nfree(pbuf, 0);
		pbuf = 0;
		psize = 0;
	}
}

PAGE* _getpg(DBinfo *DBptr)
{
	int RequiredPageSize;

	//RequiredPageSize = DBptr->DBpgsize + 16;	// Database pagesize in bytes
	RequiredPageSize = DBptr->DBpgsize + sizeof(PAGEHDR);	// Database pagesize in bytes

		if ( psize < RequiredPageSize )
	{
		if ( pbuf )
			nfree(pbuf, 0);
		pbuf = ealloc(RequiredPageSize, 0);
		psize = RequiredPageSize; 
	}
	return pbuf;
}


#endif

