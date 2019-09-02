#ifndef LERFILE_C
#define LERFILE_C

#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for flag bits

short aliasexp()
{
	RTAB	*v5;
	RTAB	*rt;
	short	RTno;
	short	expno;
	short	v4;

	// example:  "replace t1 dbname 'Tranfl1' 'tf1'"
	if ( symbol == 930 )
		loaderr(7, sym);				// "unexpected end of line"
	else
	{
		RTno = getrtmem();
		allrtpt(RTno, &rt);
		// we have table number in lerfile, now get DBname, TableName, Alias
		while ( symbol )
		{
			expno = loadexp(0, 1);		// look for string literal or variable name
			if ( !expno )
				break;					// error exit
			rt->enright = expno;
			if ( symbol == 930 )		// <CR> terminate command,
			{	
				dallrtpt(&rt);
				return RTno;			// exit_success
			}
			else	// symbol != 930	// go around again for next parameter
			{							// ** Only 3 are valid and read in clenter, but no limit on number entered **
				v4 = getrtmem();
				//if ( !v4 )
				//	 __assert_fail("tmpidx > 0", "lerfile.c", 0x2Bu, "aliasexp");
				assert(v4 > 0);
				rt->NextRT = v4;
				v5 = RTARR(rt->NextRT);
				rt = v5;
			}
		}
		dallrtpt(&rt);
	}
	return 0;			// error exit
}

int lerfile(int *a1, short OpSymbol)
{
	PTAB	*pt;
	TDesc	*TTptr;
	RTAB	*rt;

	int		TDno;
	short	expno;
	short	RTno;
	 
    pt = PTARR(getptabp((*a1)++));
    pt->OpCode		= 400;					// default to a no-op
    pt->SrcLineNo	= lla;
    symbol = getsym();
    if ( symbol != 2230 )
    {
         if ( symbol == 930 )
              loaderr(40, sym);				// "missing table/alias"
         else
              loaderr(39, sym);				// "invalid table/alias"
         return 0;
    }
    RTno = getrtmem();
    allrtpt(RTno, &rt);
    TDno = findtd(sym, -1);
    if ( TDno < 0 )
    {
        loaderr(14, sym);					// "table not open"
        dallrtpt(&rt);
		return 0;
    }
//---------------------------
	rt->TTno  = TDno;
    TTptr	  = &ttab[rt->TTno];
	pt->TABno = RTno;
	
	switch (OpSymbol)
	{
		case 55:							// "replace"	** Undocumented in CL4 GeFahr! ** ??
			rt->OpType		= 0x0800u;
			pt->OpCode		= 530;
			break;
		case 160:							// "clear"
			rt->OpType		= 0x0020u;
			pt->OpCode		= 580;
			break;
		case 300:							// "delete"
			rt->OpType		= 0x0010u;
			pt->OpCode		= 570;
			TTptr->TDlocked	|= ttl_DEL;		// 0x8000
			break;
		case 580:							// "getnext"
			rt->OpType		= 0x0002;		// OpType specifies direction: next or prev
			pt->OpCode		= 520;
			TTptr->TDlocked	|= ttl_GETNP;	// 0x0100;
			break;
		case 590:							// "getprev"
			rt->OpType		= 0x0004;
			pt->OpCode		= 515;
			TTptr->TDlocked	|= ttl_GETNP;	// 0x0100;
			break;
		case 600:							// "get"
			rt->OpType		= 0x0001;
            pt->OpCode		= 510;
            TTptr->TDlocked	|= ttl_GET;		// 0x0004;
			break;
		case 1220:							// "put"
			rt->OpType		= 0x0008;
            pt->OpCode		= 530;
            TTptr->TDlocked	|= ttl_PUT;		// 0x0002;
			break;
	}

	//---- lock / lockr ----

	symbol = getsym();
    if ( symbol == 790 )					// "lockr"
	{
        if ( rt->OpType & 0xFFF8 )			// 1111 1111 1111 1000
		{
	        loaderr(66, sym);				// "only valid with 'get','getnext','getprev' statements"
		    dallrtpt(&rt);
			return 0;
        }
        rt->LockType		|= 0x0400;
        TTptr->TDlocked		|= ttl_LOCKR;	// 0x0080;
		symbol = getsym();
    }
    else if ( symbol == 800 )				// "lock"
	{
		rt->LockType	|= 0x0200;
		TTptr->TDlocked	|= ttl_LOCK;		// 0x0001;
		symbol = getsym();
	}

	//----  all/where ----
    if ( rt->OpType & 0x0007 )				// ('get','getnext','getprev')
	{
		if ( symbol == 70 )					// all
		{
			rt->OpType |= 0x0100;
			symbol = getsym();
		}
		else if ( symbol == 1720 )			// where
		{
			symbol = getsym();
			expno = loadexp(1, 1);			// get the "where" expression
			if ( !expno )
			{
				dallrtpt(&rt);
				return 0;					// error
			}
			rt->WhereEXP = expno;
		}
	}
    else if ( rt->OpType & 0x0800 )			// replace  *EVIL*
	{
		expno = aliasexp();					// example:  "replace t1 dbname 'Tranfl1' 'tf1'"
		if ( !expno )
		{
			dallrtpt(&rt);
			return 0;
		}
		rt->NextRT = expno;
	}

	if ( symbol != 930 )			// <CR> expected to follow
    {
        loaderr(33, sym);			// unexpected symbol
        dallrtpt(&rt);
		return 0;
    }
    dallrtpt(&rt);
	return 1;						// exit_success
}



#endif
