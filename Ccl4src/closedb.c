#ifndef CLOSEDB_C
#define CLOSEDB_C

#include "DBdefs.h"
#include "cl4.h"

//=== different functionality to libcl4. No context value passed ===
int closedb(int DBno)
{
    DBase	*DTptr;
	TDesc	*TTptr;
	int		TDno;
	int		v5;
	short	DTno;
	
    TDno = 0;
    if ( no_ttabs )
    {
        if ( !DBno )
            ulockall();
        for ( TTptr = &ttab[no_ttabs - 1]; TTptr >= ttab; --TTptr )
        {
            if ( TTptr->TDrecsize && TTptr->TableAlias[0] != '>' ) // alias == '>' indicates use of "replace" command *EVIL*
            {
                if ( TTptr->TTrtd != -1 )
                {
                    TDno = rtdget(TTptr);
                    release(TDno);
                }
                TTptr->TableName[0] = 0;
                if ( TDno == no_ttabs - 1 )
                    --no_ttabs;
            }
        }
        DTno = 0;
        DTptr = dtab;
        while ( no_dtabs > DTno )
        {
			if ( DTptr->FullDBname && DTptr->DBno >= 0 )
            {
				v5 = cldbclose(DTptr->DBno);
                if ( v5 != DTptr->DBno )
					dberror(v5, DTptr->DBno, -1);
				DTptr->FullDBname = mfree_0(DTptr->FullDBname);
			}
            DTptr++;
            DTno++;
        }
    }
    return 0;
}

#endif
