#ifndef GETTDFNO_C
#define GETTDFNO_C

#include <assert.h>			// for assert() naturlich!
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"		// for bit field values

int gettdfno(ENTAB *entab, int *TTno, char *FLDtype, bool UpdateTT)		// returns FieldNo
{
    ENTAB	*entb;
    FLDdesc *fld;
    TDesc	*TTptr;
    short	Subscript;
    int		fno;
    bool	AllArray = false;	//	Set to true when no subscript, or [0] selected

    if ( !entab )
        __assert_fail("0", "gettdfno.c", 0x52u, "gettdfno");
	//assert(entab);

    if ( entab->entype == 2 && (entab->Enun.Enop.Enoper & 0xFC00) == 0xAC00 )	// Array variable. can be field in a table
    {
        entb = ENARR(entab->enleft);
        *TTno		= entb->Enun.Enref.TTno;		// 0 = Variable, non-zero = Field in a Table		** update caller mem **
        fno			= entb->Enun.Enref.VarNum;		// destination field no
        TTptr		= &ttab[*TTno];
        rtdget(TTptr);								// seems pointless, but rtdget() will throw _asserts if TTptr->rtd invalid
        fld			= &TTptr->TTfields[fno];

		Subscript = evalint(ENARR(entab->enright));	// 1 based at this point
        AllArray = (Subscript == 0);		// If Subscript is ZERO, set *all* elements to this value
 		if ( Subscript <= 0 )
            Subscript = 1;
 
		if ( *TTno )	// Is this a table variable? (normal variables hang off TDno 0)
        {
            if ( TTptr->NumFields < fno + Subscript )	// range check on fno
                Subscript = 1;
            fno = (fno + Subscript - 1);
        }
		else
		{
			if ( fld->FLDelemID < Subscript )
			    Subscript = 1;
			Subscript--;	// now ZERO based
			
			if ( fld->FLDtype == 'C' )
			{
				fld->FLDdata = &fld->FLDname[Subscript * (fld->FLDlen + 1)];	// Allow for terminating \0
			    if ( *(char *)fld->FLDdata )									// set quick-zero flag
					fld->FLDstat &= ~fld_ZERO;	// not-zero
				else
			        fld->FLDstat |= fld_ZERO;	// zero
			}
			else		// Everything else stored internally as a double
			{
				fld->FLDdata = &fld->FLDname[Subscript * sizeof(double)];
			    if ( *(double *)fld->FLDdata == 0.0 )							// set quick-zero flag
			        fld->FLDstat |= fld_ZERO;	// zero
				else
					fld->FLDstat &= ~fld_ZERO;	// not-zero
			}
		}
		*FLDtype = fld->FLDtype;				// ** update caller mem **
    }
	else if ( entab->entype == 1 )				// Return a variable value. Can be field in a table.
	{
		*TTno	= entab->Enun.Enref.TTno;		// Enun is a multi-use field depending on entab->entype
		fno		= entab->Enun.Enref.VarNum;
		TTptr	= &ttab[*TTno];
		rtdget(TTptr);							// seems pointless, but rtdget() will throw _asserts if TTptr invalid
		*FLDtype = TTptr->TTfields[fno].FLDtype;
		if ( !*TTno )							// True if this is a normal variable, Array variables can't exist in DBase tables
		{
			fld = &TTptr->TTfields[fno];
			if ( fld->FLDelemID )				// FLDelemID used as "Number of Elements" for Array variables
			{									// No subscript specified, so we are setting *all* values !!
			    AllArray = true;				// All array elements set to single value.
			    fld->FLDdata = fld->FLDname;	// Array hangs off FLDname
			}
		}
	}
	else
		return -1;	// err_exit

	fld = &TTptr->TTfields[fno];
    if ( AllArray && !*TTno && fld->FLDelemID )		// Array variables can't exist in DBase tables
        fld->FLDstat |= fld_ARRAY;	// $0002		// flag indicates this is an array variable that needs *all* values set to default
    else
        fld->FLDstat &= ~fld_ARRAY;	// $FFFD

    if ( UpdateTT && *TTno && !(fld->FLDstat & fld_DATA_AVAIL))	// update TT Workarea if this is a field in a Table, and data_avail flag not set
		rectott(*TTno, fno);	// Update data in TT Workarea
    
	return fno;
}

#endif
