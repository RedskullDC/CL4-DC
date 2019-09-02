#ifndef ISDEFINED_C
#define ISDEFINED_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for flag bits

bool lutree(VARTREE *vartree, char *VarName, int StrLen)					// search system variables for a match
{
	int v4;				

	if ( vartree )
    {
		v4 = strncmp(VarName, vartree->VarName, StrLen + 1);
		if ( v4 )
        {
			if ( v4 > 0 )
	            return lutree(vartree->VarNext, VarName, StrLen);       // recursion here
            else	// < 0
	            return lutree(vartree->VarPrev, VarName, StrLen);       // recursion here
		}
		return true;	// match
	}
    return false;		// no vartree, or at end of tree structure
}

void _define(char *StringVal)
{
     FLDdesc *fld;
	 int LitLen;										// string Literal length
	 
     LitLen = strlen(StringVal);

	 assert(getvars(last_var) == ((void *)0));
	 fld = getvarmem();									// May realloc() var mem space
	 assert(fld == getvars(last_var));
     
	 fld->FLDname	= getmem(LitLen);
     cdbcpystr(fld->FLDname, StringVal, 0);

     fld->FLDlen	= LitLen;
     fld->TDFtype	= 0x001Au;							// string type
     fld->FLDtype	= 'C';
     fld->FLDdata	= getmem(LitLen + 1);
     cdbcpystr((char *)fld->FLDdata, StringVal, 0);		// copy string to data area as well as name
     fld->FLDstat	&= ~fld_ZERO;						// clear the "quick zero" flag. Variable is NON-ZERO
}

int isdef(char *StringVal)
{
	if ( !lutree(troot, StringVal, strlen(StringVal)) )
		_define(StringVal);
	return 1;						// return value ignored
}

#endif
