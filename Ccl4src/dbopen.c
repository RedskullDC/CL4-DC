#ifndef DBOPEN_C
#define DBOPEN_C

#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"                    // Table numbers

// reserved TDno table names
// #define	td_tables		0	// "tables"
// #define	td_fields		1	// "fields"
// #define	td_locks		2	// "locks"

int cldbopen(char *DBname, ...)
{
	int v3;
	char *v5;
	va_list vl;
	char *DBnamelist[30];

	//printf("cldbopen(DBname : [%s],...)\n",DBname);
	va_start(vl, DBname);
	DBnamelist[0] = DBname;
	v3 = 1;
	v5 = va_arg(vl,char*);	// linked DB's are not used in libcl4. Second param always ZERO
	while (v5)
	{
		DBnamelist[v3++] = v5;		
		//printf("cldbopen(DBname v5 : %s\n",v5);
		v5 = va_arg(vl,char*);
	}
	va_end(vl);	// keep va macros happy
	DBnamelist[v3] = 0;	// terminate the DB list
	return cldbopenv(DBnamelist);
}

int cldbopenv(char **DBnamelist)
{
	char	**v3; 
	DBinfo	*DBptr; 

	int		ErrorCode;

	short	v6; 
	short	Version; 
	short	DBno; 

	for ( v3 = DBnamelist; *v3; v3++ )
		;
	DBptr = 0;
	ErrorCode = 0;
	v3--;				// v3 now points at last valid string
	
	if ( DBnamelist <= v3 )
	{
		while ( 1 )
		{
			DBptr = nalloc(sizeof(DBinfo), DBptr);	// Linked list of DBptrs

			if ( !DBptr || (DBptr->DBname = nalloc(lenstr(*v3) + 1, 0)) == 0)
			{
				ErrorCode = 43;
				break;
			}
			cdbcpystr(DBptr->DBname, *v3, 0);
			v6 = _oshare(*v3, (char*)&DBptr->DBopenflg);
			DBptr->DBfilehandle = v6;
			if ( v6 < 0 )
			{
				ErrorCode = 7;
				break;
			}
			Version = _rdhdr(DBptr->DBfilehandle, DBptr);
			DBptr->DBversion = Version;
			if ( Version != 1765 && Version != 1766 && Version != 1767 )
			{
				ErrorCode = Version == 1764 ? 61 : 8;
				break;
			}
			DBptr->PageList2[0] = 0;
			DBptr->PageList3[0] = 0;
			DBptr->SpareList[0] = 0;
			DBptr->PageList1[0] = 0;
			--v3;
			if ( DBnamelist > v3 )
				goto LABEL_15;		// Success exit from block
		}
		if ( DBnamelist <= v3 )
			goto LABEL_16;
	}

//---------------
LABEL_15:
	DBno = _nxtdb(DBptr);
	if ( DBno < 0 )
	{
LABEL_16:
		while ( DBptr )
		{
			if ( DBptr->DBfilehandle >= 0 )
				_relshare(DBptr);
			if ( DBptr->DBname )
				nfree(DBptr->DBname, 0);
			DBptr = nfree(DBptr, DBptr->DBnextDB);
		}
		return ErrorCode ? -ErrorCode : -9 ;
	}
//-------------------------

	_chktd(td_tables)->TDDBinfo = DBptr;		// "tables"
	_chktd(td_fields)->TDDBinfo = DBptr;		// "fields"
	if ( DBptr->DBvflag > 3 )					// only Version 4 (or later) DBases have a locks table 
		_chktd(td_locks)->TDDBinfo = DBptr;		// "locks"
	_setshut();
	return DBno;
}
#endif
