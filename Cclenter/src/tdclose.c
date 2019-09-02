#ifndef TDCLOSE_C
#define TDCLOSE_C

#include <stdio.h>
#include <assert.h>		// for assert() naturlich!
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"

void tdclose(int TTno)
{
	TDesc	*TTptr;
	FLDdesc *fld;
	Qlen	*qlen;
	int		retcode;

	TTptr	= &ttab[TTno];
	fld		= TTptr->TTfields;
	if ( fld )
	{
		assert(TTptr->TTrtd != -1);	// -1 means never opened!
		while (fld->FLDelemID)
		{
			if ( fld->FLDdata )
				mfree_0(fld->FLDdata);
			if ( fld->FLDname )
				mfree_0(fld->FLDname);
			
			qlen = fld->FLDqlen;
			if ( qlen )
			{
				if ( qlen->QfieldHd )
					mfree_0(qlen->QfieldHd);
				if ( qlen->Qdata )
					mfree_0(qlen->Qdata);
				mfree_0(qlen);
			}
			fld++;
		}
		//if ( TTptr->TTfields )	// must be true in this loop
		TTptr->TTfields = mfree_0(TTptr->TTfields);
	}
  
	if ( TTptr->TDworkarea1 )
		TTptr->TDworkarea1 = mfree_0(TTptr->TDworkarea1);
	if ( TTptr->TDworkarea2 )
		TTptr->TDworkarea2 = mfree_0(TTptr->TDworkarea2);
//=========================================
	// libcl4  functionality
	if ( TTptr->TTselect )
		TTptr->TTselect = (EXPR*)fretre(TTptr->TTselect, 0);
	// clenter functionality 
	//TTptr->TTfields = 0;            // possible memory leak here ??......
//=========================================
	TTptr->NumFields	 = 0;
	TTptr->TableAlias[0] = 0;
	TTptr->TableName[0]  = 0;
	if ( TTptr->TTrtd != -1 )
	{
		retcode = release(TTptr->TTrtd);
		if ( retcode < 0 )
			dberror(retcode, 0, TTptr->TTrtd);
	}
	TTptr->TTrtd = -1;
}

#endif

