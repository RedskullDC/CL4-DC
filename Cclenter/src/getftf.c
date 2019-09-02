#ifndef GETFTF_C
#define GETFTF_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <ctype.h>		// tolower() etc.
#include <termcap.h>
#include <stdlib.h>		// for srand48()
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for bit field values

// Find the Field pointed to by the expression, and set the fld parameters
// accordingly depending to the expression type

FLDdesc *getftf(ENTAB *ent, bool UpdateTT, FLDdesc *fld)
{
    ENTAB	*v6;
    FLDdesc *fld2;
    TDesc	*TTptr;
    short	Subscript;
    short	fno;
    int		TDno;
    bool	AllArray = false;

	// fld is provided by caller, in case necessary. Not always returned. Routine assumes that ent is correctly set.

    zap((char*)fld, sizeof(FLDdesc));
	if ( ent->entype == 0x04 )					// Float Literal - RVALUE only - **clcomp doesn't create these!**
    {
        fld->FLDdata = (void*)&dbl_0;
        fld->FLDtype = '6';
        if ( *(float *)&ent->TTno == 0.0 )		//ent->TTno should be an enum field ?!?
            fld->FLDstat |= fld_ZERO;
        dbl_0 = *(float *)&ent->TTno;
        return fld;
    }
    else if ( ent->entype == 0x08 )				// Integer literal - RVALUE only
    {
        fld->FLDdata = (void*)&dbl_1;
        fld->FLDtype = 'N';
        if ( !*(int *)&ent->TTno )
            fld->FLDstat |= fld_ZERO;
        dbl_1 = (double)*(int *)&ent->TTno;
        return fld;
    }
    else if ( ent->entype == 0x10 )				// String Literal (3chars or less) - RVALUE only
    {
        fld->FLDdata = (void*)&ent->TTno;
        fld->FLDtype = 'C';
        if ( !*(char*)&ent->TTno )
            fld->FLDstat |= fld_ZERO;
        return fld;
    }
    else if ( ent->entype == 0x01 )				// Retrieve variable or field data - LVALUE or RVALUE
    {
        fno		= ent->RecNo;
        TDno	= ent->TTno;
        TTptr	= &ttab[TDno];
        rtdget(TTptr);							// appears pointless, but throws exceptions on TTptr sanity checks.
        if ( !TDno )							// Is this a normal variable? (Hangs off TD0)
        {
            fld2 = &TTptr->TTfields[fno];
            if ( fld2->FLDelemID )				// Is this an Array variable?
            {									// No subscript specified, so.....
                AllArray = true;				// set AllArray flag for below.
                fld2->FLDdata = fld2->FLDname;	// set data pointer to Array element[0]
            }
        }
    }
    else if ( ent->entype == 2 && (unsigned short)(ent->TTno & 0xFC00) == 0xAC00 )	// 0xAC00 == Array operation - LVALUE or RVALUE
	{
		v6 = ENARR(ent->enleft);

		fno		= v6->RecNo;				// FieldNo
		TDno	= v6->TTno;
		TTptr	= &ttab[TDno];
		rtdget(TTptr);						// appears pointless, but throws exceptions on TTptr sanity checks.
		fld2	= &TTptr->TTfields[fno];

		Subscript = evalint(ENARR(ent->enright));	// get Array index, ONE based at this point
		AllArray = (Subscript == 0);				// ZERO means set all values, Set AllArray flag for below
		
		if ( Subscript <= 0 )				// range check
		    Subscript = 1;

		if ( TDno )			// table field
		{
		    if ( TTptr->NumFields < fno + Subscript )	// bounds check on Subscript
		        Subscript = 1;
		    fno = (fno + Subscript - 1);
		}
		else				// Normal variable
		{												// check for Array variables, and find offset to data.
			if ( fld2->FLDelemID < Subscript )			// bounds check on Subscript
			    Subscript = 1;
			Subscript--;								// now ZERO based

			if ( fld2->FLDtype == 'C' )
			{
				fld2->FLDdata = &fld2->FLDname[Subscript * (fld2->FLDlen + 1)];	// allow for \0 byte terminator in strings
			    if ( *(char*)fld2->FLDdata )									// set quick zero flags
					fld2->FLDstat &= ~fld_ZERO;	// 0xFFFB;	// non-zero
				else
				    fld2->FLDstat |= fld_ZERO;	// 0x0004;	// zero
			}
			else
			{
				fld2->FLDdata = &fld2->FLDname[Subscript * sizeof(double)];		// all numerics stored as doubles
			    if ( *(double *)fld2->FLDdata == 0.0 )							// set quick zero flags
				    fld2->FLDstat |= fld_ZERO;	// 0x0004;
				else
					fld2->FLDstat &= ~fld_ZERO;	// 0xFFFB;
			}
		}
	}
	else
        return 0;	// error.

//==============================================================

    fld2 = &TTptr->TTfields[fno];
    if ( AllArray && !TDno && fld2->FLDelemID )		// *Array* type *variable* with default value set. 
        fld2->FLDstat |= fld_ARRAY;		// $0002
    else
        fld2->FLDstat &= ~fld_ARRAY;	// $FFFD

	if ( UpdateTT && TDno && !(fld2->FLDstat & fld_DATA_AVAIL) )	// 0x0010 == data_avail flag
        rectott(TDno, fno);	// Update data in TT Workarea

    return fld2;
}

#endif
