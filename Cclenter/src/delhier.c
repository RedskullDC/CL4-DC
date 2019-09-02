#ifndef DELHIER_C
#define DELHIER_C

#include "DBdefs.h"
#include "cl4.h"

int delset(int TDno, int DBno, char *TableName, bool isKey)
{
    TDesc	*TTptr;
	FLDdesc *fld;
	EXPR	*expr;
	int		TTno;
	int		TDnoa;
	short	retcode;
	
    TTno = findtd(TableName, DBno);
    if ( TTno >= 0 )
    {
        TTptr = &ttab[TTno];
        TDnoa = rtdget(TTptr);
        if ( isKey )							// isKey == true if we are deleting a Key Field, 0 if a SUBrecord
		{
            tdtotd(TTno, TDno, 1, 2);			// copy current key fields from parent record.
			expr = bldexpr(TTno, -1, 0);		// -1 creates a "match all" expression
		}
		else
			expr = bldexpr(TDno, -1, 0);		// -1 creates a "match all" expression

        retcode = cdbrestrict(TDnoa, expr);
        if ( retcode < 0 )
            dberror(retcode, 0, TDnoa);
        
		while ( 1 )
        {
            if ( rdnxt(TTno, expr, 0, 0) <= 0 )		// no more records found or error
            {
                fretre(expr, 0);
                return 1;							// exit_success
            }
            if ( TTptr->TDlocked & (ttl_HASSUB | ttl_HASK))		// 0x4010  indicates we have Key/Sub tables
            {
                fld = TTptr->TTfields;
				while (fld->FLDelemID)
				{
					if ( fld->FLDtype == 'S' )
						delset(TTno, DBno, (char *)fld->FLDdata, false);	// recursive behaviour
					else if ( fld->FLDtype == 'K' )
						delset(TTno, DBno, (char *)fld->FLDdata, true);		// recursive behaviour
					++fld;
				}
            }
            
			tttorec(TTno, TTptr->TDworkarea1, 1);
            retcode = cldelr(TDnoa, TTptr->TDworkarea1, 0, TTptr);
            if ( retcode < 0 )
                dberror(retcode, 0, TDnoa);

            rfree(TTno, 0);
            TTptr->TDlocked &= ~ttl_AVAIL;		// 0xFFDFu;	clears 0x0020 ttl_AVAIL 1111 1111 1101 1111
        }
        
    }
    return 0;	// error_exit, TableName not found.
}

int delhier(int TTno, int delSubKey)	// delSubkey == kill all sub/key records
{
    TDesc	*TTptr;
	FLDdesc *fld;
	int		TDno;
	int		v11;
	short	retcode;
	
    v11		= 1;
    TTptr	= &ttab[TTno];
    TDno	= rtdget(TTptr);
    tttorec(TTno, TTptr->TDworkarea1, 1);
    
	if ( delSubKey && TTptr->TDlocked & (ttl_HASSUB | ttl_HASK))			// 0x4010 indicates we have Key/Sub tables
    {
		fld = TTptr->TTfields;
        while ( fld->FLDelemID )
        {
			if ( fld->FLDtype == 'S' )
				delset(TTno, TTptr->DBnumber, (char *)fld->FLDdata, false);
            else if ( fld->FLDtype == 'K' )
			    delset(TTno, TTptr->DBnumber, (char *)fld->FLDdata, true);	// true == isKey
			fld++;
        }
    }

	retcode = cldelr(TDno, TTptr->TDworkarea1, 0, TTptr);
    if ( retcode <= 0 )
    {
        if ( retcode < 0 )
            dberror(retcode, 0, TDno);
        v11 = 0;
    }
    ulckrec(TTno);
    rfree(TTno, 0);
    TTptr->TDlocked &= ~ttl_AVAIL;		// 0xFFDFu;	clears 0x0020 ttl_AVAIL
    return v11;
}

#endif
