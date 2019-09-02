#ifndef SAMEKEY_C
#define SAMEKEY_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <stdlib.h>		// strtod() etc.
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

// only called by join()
bool samekey(int TDno, char *Workarea, int TDno2, char *Workarea2)
{
    TDef	*tdef1;
	TDef	*tdef2;
	char	*Buffer;
	char	*src;
	int		v6;
    int		v7;

	//printf("samekey\n");
    tdef1 = tblrow(TDno);
    if ( tdef1 && tblrow(TDno2) )
    {
        src = Workarea;
        while ( tdef1->TDFtype & tdf_KEY)            // &1 == Key field
        {
            tdef2	= tblrow(TDno2);
            Buffer	= Workarea2;
			while ( tdef2->TDFentry )
			{
                if ( tdef2->TDFentry == tdef1->TDFentry )
                {
					if ( _cmpattrib(src, tdef1->TDFsize, Buffer, tdef2->TDFsize, tdef1->TDFtype)
					  || (v6 = (tdef1->TDFtype & tdf_STRING) ? _fwidth(src, tdef1->TDFsize, tdef1->TDFtype) : tdef1->TDFsize,
						  v7 = (tdef2->TDFtype & tdf_STRING) ? _fwidth(Buffer, tdef2->TDFsize, tdef2->TDFtype) : tdef2->TDFsize, v6 != v7))
						return false;	// exit_fail. The Key areas don't match
                }
                Buffer += tdef2->TDFsize;
                tdef2++;
			}
            src += tdef1->TDFsize;
            tdef1++;
        }
        return true;	// exit_success. they match
    }
    else
        return false;	// exit_fail. They Key areas don't match
}
#endif
