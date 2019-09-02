#ifndef DBGLOG_C
#define DBGLOG_C

#include "DBdefs.h"
#include "cl4.h"

//#define DB_MAX 255			// max size of open databases array in DBdefs.h

DBinfo* _chkdb(unsigned int DBno)
{
	return (DBno < DB_MAX) ? _dbase[DBno] : 0;	// return DBptr. may be null
}

signed int _nxtdb(DBinfo *DBptr)	// Store DBptr in next available slot
{
	int DBno;
	for ( DBno = 0; DBno < DB_MAX ; DBno++)
	{
		if (!_dbase[DBno])
		{
			_dbase[DBno] = DBptr;
			return DBno;
		}
	}
	return -1;	// no space available!
}                             

DBinfo* _rmdb(int DBno)
{
	DBinfo *DBptr;

	DBptr = _chkdb(DBno);
	if ( DBptr )
		_dbase[DBno] = 0;
	return DBptr;
}

int fdf2db(DBinfo *DBptr)
{
	int DBno;

	for ( DBno = 0 ; DBno < DB_MAX ; DBno++)
	{
		if ( _dbase[DBno] == DBptr )
			return DBno;	// exit_success
	}
	return 0;	// not found		 *** can't DBno == ZERO be valid though?
}

signed int _chkdbname(char *DBname)
{
	DBinfo *DBptr; 
	signed int DBno; 

	for ( DBno = 0; DBno < DB_MAX ; DBno++)
	{
		DBptr = _chkdb(DBno);
		if ( DBptr && cmpstr(DBname, DBptr->DBname))
			return DBno;	// exit_success
	}
	return -1;	// no match on DBname
}

char* _dbname(int DBno)
{
	DBinfo *DBptr; 
	//printf("_dbname(DBno : %d)\n" ,DBno);

	DBptr = _chkdb(DBno);
	return (DBptr) ? DBptr->DBname : 0;
}

#endif
