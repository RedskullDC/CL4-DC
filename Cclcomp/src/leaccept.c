#ifndef LEACCEPT_C
#define LEACCEPT_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include "DBdefs.h"
#include "cl4.h"
#include "lvarnames.h"			// for flag bits

// Normal accept type field. OpCode = 1
short leaccept(short ENTno)
{
	FLDdesc *v8;
	FLDdesc *fld;
	XTAB	*xtb;
	
	int		v7;
	int		TTno;
	short	XTno;
	short	FieldNo;
	bool	isEditable		= true;		// default assume this field is editable
	bool	comma_found		= false;	// default assume no comma in field width specifier
	char	FLDtype;

    //cdbcpystr(v16, sym, 0);		// not ever used. For debug display perhaps?
    if ( block_no_0 != cur_block )
    {
		block_no_0	= cur_block;
        got_fdf		= false;		// got "first display field"
	}
    XTno = getxtmem();
    allxtpt(XTno, &xtb);
    xtb->VarExpNo = ENTno;
	
	if ( !getadwid(xtb) )			// does this field have a default width?
	{
		dallxtpt(&xtb);
        return 0;					// error_exit
	}
	
	if ( symbol == 180 )            // ","
    {
		comma_found = true;
        symbol = getsym();
	}

	if ( comma_found || (symbol == 2230 && syml == 1) )		// if we found a comma, or a single letter
	{
		if ( sym[0] == 'R' || sym[0] == 'r' )				// Required ( trumps display field, can't be both of course )
			xtb->Flags |= xtb_REQUIRED;		// 0x0400u;

		else if ( sym[0] == 'D' || sym[0] == 'd' )			// Display Only
		{
			xtb->Flags |= xtb_DISPLAY;		// 0x0004u;
			isEditable = false;
		}
		else if ( comma_found )								// comma is only valid in width specifier if a single char follows 
		{
			loaderr(37, sym);								// "invalid field width"
			dallxtpt(&xtb);
			return 0;
		}
		symbol = getsym();
	}

	if ( isEditable && !got_fdf )							// is field editable (not display only), and have we found first editable field yet?
    {
		xtb->Flags |= xtb_FDF;				// 0x0008u;		// This is first editable field on screen
        got_fdf = true;
	}
     
	FieldNo = gettf(xtb->VarExpNo, &TTno, &FLDtype);
	assert(FieldNo >= 0);
     
	switch ( FLDtype )
    {
          case 'C':
               xtb->Flags |= xtb_CHAR;		// 0x0001;	// String
               break;
          case 'D':
               xtb->Flags |= xtb_DATE;		// 0x0002;	// Date
               break;
          case 'T':
               xtb->Flags |= xtb_TIME;		// 0x0800;	// Time
               break;
          default:
               xtb->Flags |= xtb_NUMERIC;	// 0x0200;	// Numeric type
               break;
     }
     if ( isEditable )
     {
          if ( TTno )									// Is this a record in a DBase. Variables hang of TT0
          {
               v7 = btab[cur_block].TTno;				// yes
               if ( TTno == v7 )
               {
                    v8 = &ttab[v7].TTfields[FieldNo];
                    if ( v8->FLDstat < 0 )				// Is this a Key Field?
                    {
                         xtb->Flags |= xtb_KEY;			// 0x8000u;				// yes
                         if ( v8[1].FLDstat >= 0 )		// If next field is *not* a Key field...
                              xtb->Flags |= xtb_LKF;	// 0x0040u;		// indicates that we are the *last* field in the Key area
                    }
               }
          }
    }
	// all ok, get the remaining field descriptors
	
	// field [ width [ pcol[ pline [ acolumn [ aline [ prompt ] ] ] ] ] ]
    if ( !getcol(xtb, col_PROMPT) || !getrow(xtb, row_PROMPT) || !getcol(xtb, col_ANSWER) || !getrow(xtb, row_ANSWER) || !getprmpt(xtb) )
	{
		dallxtpt(&xtb);
        return 0;	// error_exit
	}

	if ( TTno )
		fld = &ttab[TTno].TTfields[FieldNo];
	else
		fld = getvars(FieldNo);
	
	// if no specifiers found, and fld points to a normal variable, use the default variable size
	if ( !TTno && fld->FLDlen && !(((unsigned short)xtb->PLine >> 8) & 0x7E) && !(((unsigned short)xtb->PCol >> 8) & 0x7E)
       && !(((unsigned short)xtb->ACol >> 8) & 0x7E) && !(((unsigned short)xtb->ALine >> 8) & 0x7E)
       && !xtb->PCol_exp && !xtb->ACol_exp )
	{
		xtb->Width = (FLDtype == 'C') ? fld->FLDlen + 1 : sizeof(double);		// set default field length.
	}
    dallxtpt(&xtb);
    return XTno;	// exit_success
}

#endif
