#ifndef SHUTDB_C
#define SHUTDB_C

#include "DBdefs.h"
#include "cl4.h"

void* _shutlnk;		// not public in libcl4

void* _shutdb(void)
{
	int DBno = 0; // esi@1

	//printf("_shutdb()\n");
	do
	{
		cldbclose(DBno);
		DBno++;
	}
	while ( DBno < DB_MAX );
	return _shutlnk;
}

void _setshut(void)
{
	//printf("_setshut()\n");

	if ( !_shutlnk )
		_shutlnk = cdbonexit(_shutdb);
}
#endif

