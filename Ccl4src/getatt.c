#ifndef GETATTRIB_C
#define GETATTRIB_C

#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"                 // for TDFtype bit field values

DBinfo* _getattrib(int DBno, char *TableName,TD0REC *WorkArea)
{
	DBinfo *DBptr;
	
	//printf("_getattrib( DBno: x%04X (%3d),TableName = '%s', TD0REC: x%08X)\n", DBno, DBno,TableName, WorkArea);

	DBptr = _chkdb(DBno);
	if ( DBptr )
	{
		cdbcpystr(WorkArea->TableName, TableName, 0);
		if ( !_chkname(DBptr, WorkArea) && DBptr )			// if chkname returns 1, Workarea held a system table name, "tables","fields" or "locks"
		{													// _chkname also sets fields in WorkArea.
			do
			{
				_chktd(td_tables)->TDDBinfo = DBptr;
				if ( getr(td_tables, (char*)WorkArea, My_pos) > 0 )	// if > 0, we found it. return DBptr that it was in
					break;
				DBptr = DBptr->DBnextDB;					// chained DB's. Haven't seen this in real world
			}
			while ( DBptr );
		}
	}
	return DBptr;	// 0 == not found, else DBptr
}

// reserved TDno table names
// #define	td_tables		0	// "tables"

#endif
