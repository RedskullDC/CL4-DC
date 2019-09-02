#ifndef ULOCK_C
#define ULOCK_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"                 // for bit field values

//#define	td_locks		2	// "locks"

//#define _LARGEFILE64_SOURCE

void _ulock(DBinfo *DBptr, int PID)
{
	char	*v2;
	EXPR	*Expr;
	TD2REC	KeyDefs;
	
	//printf("_ulock(x%08X, %d)\n", DBptr,PID);
	if ( DBptr->DBvflag > 3 )						// Only a "locks" table on V4 DBases!
	{
		_chktd(td_locks)->TDDBinfo = DBptr;
		v2 = (char*)ltoms((int*)&KeyDefs, PID);		// temp use of Keydefs mem space
		Expr = newexpr(115, 0x10u, v2, 4);			// Look for matches with our PID  [ 115 = PID field in locks record, 0x10 == equal to operator ]
		
		cdbrestrict(td_locks, Expr);

		while ( nxtr(td_locks, (char*)&KeyDefs, 0, Expr) > 0 )
			_lockr(DBptr, KeyDefs.TableIndex, KeyDefs.KeyBuff, 128, 0);

		fretre(Expr, 0);
	}
}

//  Locks Table
//  No. Type  Description
// [ 1]  *B   106			Table Index
// [ 2]  *C   116 [128]
// [ 3]   N   115			PID

void ulockall(void)
{
	DBinfo	*DBptr;
	int		DBno;
	
	for (DBno = 0 ; DBno < DB_MAX ; DBno++)
	{
		DBptr = _chkdb(DBno);
		if ( DBptr )
			_ulock(DBptr, getpid());
	}
}

#endif
