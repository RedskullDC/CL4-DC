#ifndef CL_FORK_C
#define CL_FORK_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include <termio.h>		// for ioctl etc.
#include <unistd.h>		// for isatty(fileno(stdout))
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"                    // Table numbers

// reserved TDno table names
// #define	td_tables		0	// "tables"
// #define	td_fields		1	// "fields"
// #define	td_locks		2	// "locks"


int _ltransfer(int DBno, int NewPID)				// transfer lock records to new PID after a fork()
{
    DBinfo	*DBptr;
	EXPR	*expr;
	int		*v6; // eax@4

    int		result; // eax@1
    int		v4; // eax@2
    int		PID; // eax@4
    TD2REC	td2; // [sp+20h] [bp-A8h]@4

    DBptr = _chkdb(DBno);                        
    result = 0;
    if ( DBptr )
    {
        v4 = check_bup_sem(DBptr);
        if ( v4 )
            derror(v4, DBptr, 0);
        PID = getpid();
        v6 = ltoms((int *)&td2.TableIndex, PID);	// temporary use of TD2 area for Expression Data

        for ( expr = newexpr(115, 0x10, (char *)v6, 4); DBptr; DBptr = DBptr->DBnextDB )	// ** see below for locks descrip **
        {
            if ( DBptr->DBvflag > 3 )	// only V4 DBases have a locks table
            {
                _chktd(td_locks)->TDDBinfo = DBptr;
                cdbrestrict(td_locks, expr);
                while ( getnxt(td_locks, &td2.TableIndex, expr) > 0 )
                {
                    ltoms(&td2.PID, NewPID);
                    putr(td_locks, &td2.TableIndex, 0);
                }
            }
        }
        fretre(expr, 0);
        result = 1;			// success
    }
    return result;
}

/*void ltransfer(int DBno, int a2)
{
    xenter((int)ltransfer, DBno, a2);
}
*/

void clfork(int DBno, int a2, int NewPID)
{
    DBinfo *DBptr; // eax@1

    DBptr = _chkdb(DBno);
    if ( a2 )
    {
        DBptr->PageList1[0] = 0;
    }
    else
    {
        DBptr->SpareList[0] = 0;
        _ltransfer(DBno, NewPID);		// Transfer lock records to new PID , should call through xenter!
    }
}
/*

-> d locks
 No. Type  Description
[ 1]  *B   106					// Table index offset of current table
[ 2]  *C   116 [128]			// Holds a copy of the locked record Key Area [max 128]
[ 3]   N   115					// current user PID
key = [129] record = [133]
*/

#endif

