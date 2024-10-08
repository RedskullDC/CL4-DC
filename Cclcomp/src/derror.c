#ifndef DERROR_C
#define DERROR_C

#include <stdio.h>
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
	int DBno;
	int TDno;

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

// ***********  versions for Clcomp, Clenter and LIBCL4 are different  **********  !!!!
/*
void dberror(int ErrorCode, int DBno, int TDno)
{
	char *v3;
	char v5;
	int Context;
	char *v7;
	char *v8;

	v8 = 0;
	v7 = 0;
	Context = -1;
	v3 = "DBASE";
	if ( DBno < 0 )
		v3 = "INTERNAL";
  
	eprint("\n\a%s err %d\n", v3, -ErrorCode);
	if ( DBno >= 0 )
	{
		if ( TDno >= 0 )
		{
			v8 = dtab[ttab[TDno].DBnumber].FullDBname;
			switch (TDno)
			{
			case 0:
				v7 = "tables";
				break;
			case 1:
				v7 = "fields";
				break;
			case 2:
				v7 = "locks";
				break;
			default:
				v7 = ttab[TDno].TableName;
				break;
			}
			Context = ttab[TDno].DBcontext;
		}
		else
		{
			v8 = dtab[DBno].FullDBname;
			Context = dtab[DBno].DBcontext;
		}
	}
	
	if ( v8 )
	{
		eprint("Database: [%s] ", v8);
		if ( v7 )
			eprint("Table: [%s]", v7);
		//eprint(" ctx=%d\n", Context);
		eprint("\n");
	}

	//cllogerr(ErrorCode, pnameArr[Context - 10000], v8, v7);
	//closetlog(Context, 1);
	//exit(1);                                      // dead end error exit

	cllogerr(ErrorCode, pname, v8, v7);
	//return kill(getpid(),5);
	//kill(getpid(),5);
	exit(1);

}
*/
void dberror(int ErrorNo, int DBno, int TDno)
{
	TDesc	*TTptr;
	char	*TABname;
	char	*DBname;
	signed	int v6;

	DBname		= 0;
	TABname		= 0;

	eprint("\n\a%s err %d\n", DBno < 0 ? "INTERNAL" : "DBASE", -ErrorNo);

	if ( DBno >= 0 )
	{
		DBname = dtab[DBno].FullDBname;
		if ( TDno >= 0 )
		{
			switch (TDno)
			{
			case 0:						// 0-2 are system tables
				TABname = "tables";
				break;
			case 1:
				TABname = "fields";
				break;
			case 2:
				TABname = "locks";		// V4 DBases only have locks table!
				break;
			default :
				for (TTptr = ttab, v6 = 0; v6 < no_ttabs; v6++,TTptr++)
				{
					if (TTptr->TTrtd == TDno)
					{
						TABname = TTptr->TableName;
						DBname = dtab[TTptr->DBnumber].FullDBname;
						break;
					}
				}
				break;
			}

			if ( !TABname )
			{
				 DBname = "NOT FOUND";
				 TABname = "NOT FOUND";
			}
		}
	}

	if ( DBname )
	{
		 eprint("Database: [%s] ", DBname);
		 if ( TABname )
			  eprint("Table: [%s]", TABname);
		 eprint("\n");
	}
	cllogerr(ErrorNo, ename, DBname, TABname);
	exit(1);
}

void cdberror(int a1, int DBno, int TTno)
{
	dberror(-a1, DBno, TTno);
}
#endif
