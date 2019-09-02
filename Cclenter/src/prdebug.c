#ifndef prdebug_C
#define prdebug_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <sys/types.h>	// for umask()
#include <sys/stat.h> 
#include "DBdefs.h"
#include "cl4.h"

// Not implemented, web only:

int dbfd = -1;			// default to debug file not open

void lockdebug(int LockMode)
{
	if ( lseek64(dbfd, 0LL, 0) < 0 || lockf64(dbfd, LockMode, 1LL) < 0 )
		dberror(-123, -1, 0);	// dead end error condition
}

void opendebug(void)
{
	char *v1;
	char *v2;
	__mode_t umask_sav;

	if ( dbfd < 0 )
	{
		v1 = getevar("CLDEBUG");
		v2 = strchr(v1, ':');
		if ( !v2 || (*v2 = 0, !v2[1]) )
		{
			if ( !isCGI )
				*v1 = 0;
		}
		
		if ( *v1 )
		{
			umask_sav = umask(0);
			dbfd = open64(v1, 'B');
			umask(umask_sav);

			if ( dbfd < 0 )
			{
				syserror("can't open debug file - %s", v1);
				exit(1);
			}
		}
		else
			debuglogging = 0;
	}
}

// used by debug routines 

char	buf[1024];
char	buf1[1024];
char	buf2[1024];
int		l;
int		l1;
int		l2;

void prdebcon(int LineNo, bool Condition, int CurrPTno)		// debug show condition : TRUE or FALSE
{
    BTAB	*btb;
	short	cy_sav;
	short	cx_sav;
	int		ErrorCode = 0;      
	char	Buffer[128];
	
    if ( debuglogging && dbfd < 0 )
		opendebug();
    
	for ( btb = btab; btb->BlockName[0]; btb++ )
    {
        if ( CurrPTno >= btb->StartLine && CurrPTno <= btb->EndLine )
            break;
    }
	if ( isCGI || debuglogging )
	{
		sprintf(buf, "[%d] %s: [%s] ", getpid(), pname, btb->BlockName);
		sprintf(buf1, "ln %d, condition %s\n", LineNo, (Condition ? "true" : "false"));
		l	= strlen(buf);
		l1	= strlen(buf1);
		lockdebug(1);
		if ( lseek64(dbfd, 0LL, SEEK_END) < 0 || write(dbfd, buf, l) != l || write(dbfd, buf1, l1) != l1 )
			ErrorCode = -121;
		lockdebug(0);
		if ( ErrorCode )
		  dberror(ErrorCode, -1, 0);
	}
	else
	{
		cx_sav = _cx;	// save current x/y values, and restore after debug line display
		cy_sav = _cy;
		qat(1, _li);                 // first char of last line
		eprint("[%s] ln %d, condition %s ", btb->BlockName, LineNo, (Condition ? "true" : "false"));

		if ( linput(Buffer, 0, 0) )	// wait for a keypress.
			debug = 0;

		_cx = 1;
		_cy = _li;
		erase_line();	// wipe debug message, and return pointer to previous location

		_cx = cx_sav;
		_cy = cy_sav;
	}
}

void prdebass(int LineNo, ENTAB *entab, int CurrPTno)	// debug show assignment : VARIABLE = VALUE
{
    BTAB	*btb;
    TDesc	*TTptr;
    FLDdesc *flda;
    
    int		FieldNo2;
	int		ErrorCode = 0;      
    int		TTno;
	short	cy_sav;
    short	cx_sav;
    char	Buffer[2];
    char	a3[1032];
	char	FLDtype;
	
    if ( debuglogging && dbfd < 0 )
		opendebug();
    
	for ( btb = btab; btb->BlockName[0]; btb++ )
    {
        if ( CurrPTno >= btb->StartLine && CurrPTno <= btb->EndLine )
            break;
    }

	if ( isCGI || debuglogging )
	{
		sprintf(buf, "[%d] %s: [%s] ", getpid(), pname, btb->BlockName);
		if (entab->entype == 1 || entab->entype == 2)
		{
			FieldNo2 = (signed short)gettdfno(entab, &TTno, &FLDtype, 0);
			TTptr	 = &ttab[TTno];
			flda	 = &TTptr->TTfields[FieldNo2];

			if ( TTno )
				sprintf(buf1, "ln %d, %s.%d = ", LineNo, TTptr->TableAlias, FieldNo2 + 1);			// Table field
			else
				sprintf(buf1, "ln %d, %sariable = ", LineNo, (flda->FLDelemID ? "array v": "v"));	// Normal variable or Array variable

			if ( flda->FLDtype == 'C' )
				sprintf(buf2, "'%s'\n", convstr(fldtobuf(a3, flda, 0), true));	// encode contrcol chars
			else
				sprintf(buf2, "%s\n", convstr(fldtobuf(a3, flda, 0), true));
		}
		else
		{
			sprintf(buf1, "ln %d, constant = ", LineNo);
			switch (entab->entype)
			{
				case 4:
					sprintf(buf2, "(float) %f", *(float *)&entab->TTno);
					break;
				case 8:
					sprintf(buf2, "(long) %ld", *(int*)&entab->TTno);
					break;
				case 16:
					sprintf(buf2, "(text) '%s'", (char *)&entab->TTno);
					break;
				default:
					sprintf(buf2, "type=%#o, oper=%#o", entab->TTno, entab->entype);	// check order of variables!!!
					break;
			} 
		}
		
		l	= strlen(buf);
		l1	= strlen(buf1);
		l2	= strlen(buf2);
		
		lockdebug(1);
		if ( lseek64(dbfd, 0LL, SEEK_END) < 0 || write(dbfd, buf, l) != l || write(dbfd, buf1, l1) != l1 || write(dbfd, buf2, l2) != l2 )
			ErrorCode = -121;
		lockdebug(0);
		
		if ( ErrorCode )
			dberror(ErrorCode, -1, 0);	// dead end exit
	}
	else
	{
		cx_sav = _cx;	// save current x/y, restore later
		cy_sav = _cy;
		qat(1, _li);	// column 1, last line of screen

		if (entab->entype == 1 || entab->entype == 2)
		{
			FieldNo2 = (signed short)gettdfno(entab, &TTno, &FLDtype, 0);
			TTptr	 = &ttab[TTno];
			flda	 = &TTptr->TTfields[FieldNo2];
			eprint("[%s] ", btb->BlockName);
			
			if ( TTno )
				eprint("ln %d, %s.%d = ", LineNo, TTptr->TableAlias, FieldNo2 + 1);			// Table field
			else
				eprint("ln %d, %sariable = ", LineNo, (flda->FLDelemID ? "array v": "v"));	// Normal variable or Array variable

			eprint("%s ", fldtobuf(a3, flda, 0));
		}
		else
		{
			eprint("[%s] ln %d, constant = ", btb->BlockName, LineNo);
			switch (entab->entype)
			{
				case 0x04:
					eprint("(float) %f", *(float *)&entab->TTno);
					break;
				case 0x08:
					eprint("(long) %ld", *(int*)&entab->TTno);
					break;
				case 0x10:
					eprint("(text) '%s'", (char *)&entab->TTno);
					break;
				default:
					eprint("type=%#o, oper=%#o", entab->TTno, entab->entype);	// check order of variables!!!
					break;
			} 
		}

		if ( linput(Buffer, 0, 0) )		// wait for keypress
			debug = 0;

		_cx = 1;						// left side of screen
		_cy = _li;						// last line
		erase_line();

		_cx = cx_sav;					// restore x/y screen values
		_cy = cy_sav;
	}
}

#endif
