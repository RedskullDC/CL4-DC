#ifndef DISPLAY_FIELD_C
#define DISPLAY_FIELD_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>		// for drand48(), strtod()
#include "DBdefs.h"
#include "cl4.h"

void dispfld(XTAB *xtab, FLDdesc *fld, bool show, bool forceUpdate, bool allow_mask, bool use_curr_xy)
{
	// allow_mask == true means honour any relative flag bits in col/line values

	char	*MessageString;
	int		MaxLen;
	short	Column;
	short	Line;
	int		TTno;
	short	Attr;
    char	a3[1032];
	char	FLDtype;
	
    MaxLen = getwid(xtab);
    if ( MaxLen )
    {
        gettdfno(ENARR(xtab->VarExpNo), &TTno, &FLDtype, 1);
        if ( fld->FLDtype == 'C' )
			MaxLen = (fld->FLDlen > MaxLen) ? MaxLen : fld->FLDlen ;

		if ( use_curr_xy )				// Only true when called from redisplay()
        {
            Line	= xtab->C_Y;
            Column	= xtab->C_X;
        }
        else
            clgetyx(xtab, &Column, &Line, yx_answer);	// position to "answer" field [** yx_answer = global variable **]

        Attr = xtab->Attr;
        if ( show )						// show field on screen
        {
            fldtobuf(a3, fld, MaxLen);
            if ( fld->FLDtype == 'D' && strlen(a3) > MaxLen )
            {
                MaxLen		= 10;		// max length for dates is 10 chars "dd/mm/yyyy"
                xtab->width = 10;
            }
            if ( rawdisplay )			// ** rawdisplay = global variable **
                MessageString = a3;
            else
                MessageString = convstr(a3, true);	// Encode control codes into the text buffer : '\n' -> 0x0D etc.
        }
        else							// else clear the space on screen where this field is displayed
        {
            getnats(xtab->Attr, 1);		// ** seems pointless, return value ignored? **
            Attr = 0x3Fu;				// turn all attributes off?
            if ( Column <= 0 )
                Column = 1;
            fillbuf(a3, MaxLen, ' ');	// fill with <SPC>
            a3[MaxLen] = 0;
            MessageString = a3;
        }
        toscreen(Column, Line, allow_mask, MaxLen, MessageString, Attr, 1, forceUpdate);	// copy buffer to screen
    }
}

#endif

