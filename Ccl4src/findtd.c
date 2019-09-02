#ifndef FINDTD_C
#define FINDTD_C

#include <stdio.h>
#include "DBdefs.h"
#include "cl4.h"

// Different functionality to libcl4.  No Context involved, since our TD list is private.
int findtd(char *TableName, int DBno)
{
    TDesc	*TTptr;
    int		TDno;
	int		MaxLen;
	
	// printf("findtd(%s,%d)\n",TableName,DBno );
    if ( TableName )
    {
        MaxLen = strlen(TableName) + 1;
        if ( DBno >= 0 )	// Set DBno to -1 to look for TableName in all open DB's
        {
			for (TTptr = ttab, TDno = 0 ; no_ttabs > TDno ; TDno++, TTptr++)
			{
				if ( TTptr->DBnumber == DBno && cmpbuf(TableName, TTptr->TableAlias, MaxLen) || !compare(TableName, TTptr->TableName) )
					return TDno;
			}
        }
		// look for TableName in all open DB's, but only check NON-SYSTEM TD's!  (ignore 0,1,2)
		for (TTptr = ttab + 3, TDno = 3 ; no_ttabs > TDno ; TDno++, TTptr++)
		{
			if ( TTptr->TableName[0] && cmpbuf(TableName, TTptr->TableAlias, MaxLen) || !compare(TableName, TTptr->TableName) )
				return TDno;
		}
    }
    return -1 ;	// error_exit : not found!
}
#endif
