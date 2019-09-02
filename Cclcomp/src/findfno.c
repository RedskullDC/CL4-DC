#ifndef FINDFNO_C
#define FINDFNO_C

#include <stdlib.h>		// for strtol()
#include <ctype.h>		//

int findfno(int TDno, char *FieldName)
{
    FLDdesc		*fld;
    TDesc		*TTptr;
    int			FieldNo;
    char		WildName[64];

	TTptr = &ttab[TDno];

	 // if a 3 digit number is the argument, use that as the FieldNo!  isdigit() == 0x0800   *** undocumented ***
	if ( isdigit(*FieldName) && strlen(FieldName) <= 3 && (FieldNo = strtol(FieldName, 0, 10), FieldNo > 0) && TTptr->NumFields >= FieldNo )
		FieldNo--;	// Field numbers ZERO based internally
	else
    {
		fld = TTptr->TTfields;
        FieldNo = 0;
		while ( fld->FLDelemID )
        {
			if ( !compare(fld->FLDname, FieldName))
				return FieldNo;
			FieldNo++;
			fld++;
		}

		// look for a possible wildcard match on the name, if 'exact' not set to true. exact = global goodness!
		if ( !exact )
		{
			cdbcpystr(WildName, FieldName, "*", 0);	// tack a '*' onto the filename, match all
			fld = TTptr->TTfields;
			FieldNo = 0;
			while ( fld->FLDelemID )
			{
				if ( !compare(fld->FLDname, WildName))
					return FieldNo;
				FieldNo++;
				fld++;
			}
		}
		FieldNo = -1;	// not found
	}
    return FieldNo;
}

#endif
