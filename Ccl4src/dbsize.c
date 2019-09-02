#ifndef DBSIZE_C
#define DBSIZE_C

#include "DBdefs.h"
#include "cl4.h"

// Return DBase size in Number of Pages

int dbsize(int DBno)
{
	DBinfo *DBptr;

	DBptr = _chkdb(DBno);
	return DBptr ? DBptr->DBmaxpages : 0 ;
}

// return the PageSize in bytes for DBno passed.
// Optionally populate the NumTables and MaxTables fields
int dbpgsize(int DBno, short *NumTables, short *MaxTables)
{
	DBinfo *DBptr;

	DBptr = _chkdb(DBno);
	if ( DBptr )
	{
		if ( NumTables )
			*NumTables = DBptr->DBnumtables;
		
		if ( MaxTables )
			*MaxTables = DBptr->DBmaxtables;
		
		return DBptr->DBpgsize;
	}
	return 0;	// error_exit
}

#endif
