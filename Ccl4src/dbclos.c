#ifndef DBCLOSE_C
#define DBCLOSE_C

#include "DBdefs.h"
#include "cl4.h"

int _cldbclose(int DBno)
{
	DBinfo	*DBptr;
	DBinfo	*NextDB;

	DBptr = _rmdb(DBno);
	while ( DBptr )					// haven't actually seen any chained databases in real life!
	{
		_ulock(DBptr, getpid());
		_shuttd(DBptr);
		_rmlock(DBptr, DBptr->PageList1);
		
		if ( DBptr->PageList1[0] )				// mark any spare/ unused allocated pages as free in the bitmap
			_mkfree(DBptr, DBptr->PageList1);
		if ( DBptr->SpareList[0] )
			_mkfree(DBptr, DBptr->SpareList);
		
		_relshare(DBptr);
		close(DBptr->DBfilehandle);
		NextDB = DBptr->DBnextDB;	// Save nextDB value before freeing DBptr
		nfree(DBptr->DBname, 0);
		nfree(DBptr, 0);
		DBptr = NextDB;
	}
	return DBno;
}


int cldbclose(int DBno)
{
  //return xenter(cldbclose, DBno);
  return _cldbclose(DBno);
}

// appears uncalled in libcl4 (is used in clenter)
void cldbcloseall()
{
	int DBno; // esi@1

	DBno = 0;
	do
	{
		if ( _chkdb(DBno) )
			cldbclose(DBno);
		DBno++;
	}
	while ( DBno < DB_MAX );
}

#endif
