#ifndef DERROR_C
#define DERROR_C

#include <stdio.h>
#include <signal.h>		// for kill()
#include "DBdefs.h"
#include "cl4.h"

void _derror(short ErrNo)
{
    //char Buffer[16]; // [sp+10h] [bp-18h]@1

    //Buffer[itob(Buffer, ErrNo, 10)] = 0;
    //printf("Data File Error # [%s]\n", Buffer);

// *** to do - Add itob() ***
	
	printf("Data File Error # [%d]\n", ErrNo);
    sleep(1u);
    abort();
}

void derror(short ErrorCode, DBinfo *DBptr, TDinfo *TDptr)
{
	int DBno; // [sp+4h] [bp-14h]@2
	signed int TDno; // [sp+8h] [bp-10h]@2

	if ( DBptr )
	{
		TDno = -1;
		DBno = fdf2db(DBptr);
	}
	else
	{
		if ( TDptr )
			TDno = nr2td(TDptr);
		else
			TDno = -1;
		DBno = 0;
	}
	cdberror(ErrorCode, DBno, TDno);
	// cdberror never returns on LIBCL4
	_derror(ErrorCode);
}

// versions for Clenter and LIBCL4 are different!!!!

void dberror(int ErrorCode, int DBno, int TDno)
{
	char *TDname = 0;
	char *DBname = 0;
	int Context  = -1;

	eprint("\n\a%s err %d\n", (DBno < 0 ? "INTERNAL" : "DBASE" ) , -ErrorCode);
	if ( DBno >= 0 )
	{
		if ( TDno >= 0 )
		{
			DBname = dtab[ttab[TDno].DBnumber].FullDBname;
			switch (TDno)
			{
			case 0:
				TDname = "tables";
				break;
			case 1:
				TDname = "fields";
				break;
			case 2:
				TDname = "locks";
				break;
			default:
				TDname = ttab[TDno].TableName;
				break;
			}
			Context = ttab[TDno].DBcontext;
		}
		else
		{
			DBname = dtab[DBno].FullDBname;
			Context = dtab[DBno].DBcontext;
		}
	}
	
	if ( DBname )
	{
		eprint("Database: [%s] ", DBname);
		if ( TDname )
			eprint("Table: [%s]", TDname);
		//eprint(" ctx=%d\n", Context);		// libcl4 only
		eprint("\n");
	}

	//cllogerr(ErrorCode, pnameArr[Context - 10000], DBname, TDname);
	//closetlog(Context, 1);
	//exit(1);                                      // dead end error exit
	
	closescr();							// restore previous TTY seetings if necessary
	cllogerr(ErrorCode, pname, DBname, TDname);
	kill(getpid(),5);					// kill self

}

void cdberror(int a1, int DBno, int TTno)
{
	dberror(-a1, DBno, TTno);
}
#endif
