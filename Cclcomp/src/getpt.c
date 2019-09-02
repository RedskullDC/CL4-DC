#ifndef GETPT_C
#define GETPT_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <assert.h>
#include "DBdefs.h"
#include "lvars.h"

char *getmem(int MemSize)
{

	if ( MemSize & 1 )
		MemSize++;
	if ( MemSize & 2 )
		MemSize += 2;                              // round *up* to nearest 4 bytes
	return (char *)mmalloc(MemSize);
}

FLDdesc* getvarname(char *VariableName)
{
     int FieldNo;
     FLDdesc *fld;

     FieldNo = Nvars - 1;
     fld = &ttab->TTfields[FieldNo];
     while ( FieldNo > 20 )							// <=20 are pre-defined system vars
     {
        if ( !strcmp(VariableName, fld->FLDname) ) // 0 == match
			return fld;	// exit_success
        --fld;
        --FieldNo;
     }
     return 0;		// not found
}

inline FLDdesc *getvars(int Varnum)
{
	//printf("getvars(%d)\n",Varnum);
	return ( Varnum < Nvars ) ? &ttab->TTfields[Varnum] : 0;
}

FLDdesc *getvarmem(void)
{
	int varno;
	
	if ( !Nvars )
		ttab->TTfields = (FLDdesc *)mmalloc(0);	// avoid a NULL ptr error
	
	varno = Nvars++;
	ttab->TTfields = (FLDdesc *)mrealloc(ttab->TTfields, sizeof(FLDdesc) * varno, sizeof(FLDdesc) * Nvars);	// 24 = sizeof(FLDdesc)
	
	return &ttab->TTfields[varno];
}

#endif
