#ifndef GETPROMPT_C
#define GETPROMPT_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>         // for strtol()
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"

void fixstr(char *a1)	// called by getprmt. removes any unprintable chars
{
	while (*a1)
	{
		if ( (unsigned char)(*a1 - 94) <= 1u )		//  94 = '^'  ctrl char specifier
			*a1 = ' ';
		a1++;
	} 
}

XTAB *getprmpt(XTAB *xtab)
{
	FLDdesc *fld;
	ENTAB	*v8;
	short	fno;
	short	ExpNo;
	short	v4;
	int		TDno;
	char	FLDtype;
	char	nptr[45];
	
	if ( symbol == 930 )		// <CR>
	{
		  if ( !xtab->VarExpNo )
		  {
			   loaderr(7, sym);
			   return 0;
		  }
		  fno = gettf(xtab->VarExpNo, &TDno, &FLDtype);
		  assert(fno >= 0);

		  if ( TDno )
			   fld = &ttab[TDno].TTfields[fno];	// Field in a table
		  else
			   fld = getvars(fno);				// Normal variable

		  cdbcpystr(nptr, fld->FLDname, 0);			// Use the FieldName
		  fixstr(nptr);
		  ExpNo = getenmem();
		  allenpt(ExpNo, &v8);
		  v4 = putconst(nptr, 'C');
		  if ( v4 < 0 )
		  {
			   dallenpt(&v8);
			   return 0;
		  }
		  v8->Enun.Enref.VarNum	= v4;
		  v8->Enun.Enref.TTno	= 0;		// normal variable
		  v8->entype			= 1;
		  dallenpt(&v8);
		  xtab->Prompt_exp		= ExpNo;
		  return xtab;
	}
	if ( symbol == 2220 )
		fixstr(sym);
	ExpNo = loadexp(0, 1);
	if ( ExpNo )
	{
		xtab->Prompt_exp = ExpNo;
		return xtab;
	}
	return 0;
}

#endif
