#ifndef LEPRINT_C
#define LEPRINT_C

#include <stdlib.h>         // for strtod(), strtol()
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"

short leprint(void)
{
    ENTAB	*entb;
    PRTAB	*v17;
    PRTAB	*pr;
    FLDdesc *fld;

    short	ExpNo;
	register short PRno;
	int		v4;
	int		v7;
	int		fno;
    int		TDno;
	short	ENTno;
	short	a1;
	unsigned short v25;
	bool	isVariable;
	
	if ( symbol == 930 )			// <CR>. Error, print needs at least 1 arg
    {
		loaderr(7, sym);			// "unexpected end of line"
        return 0;
	}
    a1 = getprmem();
    allprpt(a1, &pr);

	if ( symbol == 930 )			// should be unreachable, tested for 930 above
    {
		dallprpt(&pr);
        return a1;
	}

	while ( symbol )
    {
		isVariable	= false;
        TDno		= 0;

		switch (symbol)
		{
		case 2200:					// 2200 = integer literal
		case 2210:					// 2210 = floating point literal
		case 2220:					// 2220 = string literal eg. 'Simple String'
            if ( loadsym(sym) == 1340 )				// 1340 = All reserved system string names are 1340
            {
				pr->ReservedFlags = loadresv(sym);	// returns a bitmask
                symbol = getsym();
			}
            else
            {
				ExpNo = loadexp(0, 1);
                if ( !ExpNo )
				{
					dallprpt(&pr);	// error_exit
					return 0;
				}
				pr->VAR_ENTno = ExpNo;
			}
			break;

		case 2230:					// 2230 = Variable or Table Name
			if ( nxtch() != '.' )	// if a ".", then we expect a fieldname to follow
		    {
				v4 = findtd(sym, -1);
	            if ( v4 > 0 )		// This is a table variable
	            {
					TDno = v4;		// We want to display *all* fields in the record
	                symbol = getsym();
	                break;
				}
			}
	        ExpNo = loadexp(0, 1);	// Normal variable, or single Table field here
	        if ( !ExpNo )
			{
				dallprpt(&pr);	// error_exit
				return 0;
			}
			pr->VAR_ENTno = ExpNo;
	        pr->WIDTH_FLAGS = 0x0400u;				// normal variable flag
	        isVariable = true;						// examine later for Datefield checking
			break;
		default:
			switch ( symbol )
            {
				case 150:							// "cl" clear line
					pr->ReservedFlags = 0x0001;
                    break;
				case 520:							// "ff"  <form feed>
					pr->ReservedFlags = 0x0002;
                    break;
				case 770:							// "nl"  <newline>
					pr->ReservedFlags = 0x0004;
                    break;
				case 250:							// "cr"
					pr->ReservedFlags = 0x1000;
                    break;
				case 405:							// "eof"   <end of file>?
					pr->ReservedFlags = 0x2000;
                    break;
				default:
					ExpNo = loadexp(0, 1);			// loads the next symbol for us
                    if ( !ExpNo )
					{
						dallprpt(&pr);				// error_exit
						return 0;
					}
					pr->VAR_ENTno = ExpNo;
                    break;
			}
            if ( pr->ReservedFlags )
                symbol = getsym();
			break;
		}

// ==== look for any width, justification specifiers ====
		if ( symbol == 180 )				// "," comma
        {
			v7 = nxtch();
			if ( v7 == ' ' || v7 == '\n' )
			{
				pr->WIDTH_FLAGS |= 0x800u;
				symbol = getsym();
			}
			else
			{
				symbol = getsym();
				if ( syml == 1 )
				{
					switch (sym[0])
					{
					case 'c':			// centred
						symbol = 170;
						break;
					case 'l':			// left-aligned
					case '^':
						symbol = 780;
						break;
					case 'r':			// right-aligned
					case '$':								// $ = 340, rest are fudged
						symbol = 340;
						break;
					}
				}

				if ( symbol == 780 || symbol == 170 || symbol == 340 ) // field justification
				{
					if ( !TDno )					// normal variable, already set above
						pr->WIDTH_FLAGS = 0x0400u;
					   
					if ( symbol == 170 )			// centred
						pr->WIDTH_FLAGS |= 0x1000;
					
					else if ( symbol == 340 )		// right-aligned
						pr->WIDTH_FLAGS |= 0x2000;
					   
					else if ( symbol == 780 )       // left-aligned
						pr->WIDTH_FLAGS |= 0x4000;
					   
					symbol = getsym();
				}
				else if ( symbol == 2200 || symbol == 2210)					// Integer or FP Literal
				{			// we are looking for a 1-3 digit number at this point
					pr->WIDTH_FLAGS = strtol(sym, 0, 10);
					symbol = getsym();
				}
				else if ( symbol != 930 )			// not <CR>
				{
					ExpNo = loadexp(0, 1);
					if ( !ExpNo )
					{
						dallprpt(&pr);	// error_exit
						return 0;
					}
					pr->WIDTH_ENTno = ExpNo;		// Width expression trumps flags
					pr->WIDTH_FLAGS = 0;
				}
			}
		}
		else	// no comma found, use default width
		{
			pr->WIDTH_FLAGS |= 0x800u;				// use default width flag?
		}

//================================

		if ( isVariable && yflag )					// yflag == command line option to "display widths of 'D' type input/output fields"
			prdatefield(pr->VAR_ENTno, 0, pr);
		
		if ( TDno )			// Are we looking at a table record? True if we want to print all records in the table 
        {
			v25 = pr->WIDTH_FLAGS & 0x83FF;			// 1000 0011 1111 1111
            fld = ttab[TDno].TTfields;
            fno = 0;
            while ( fld->FLDelemID )
            {
				ENTno	= getenmem();				// get new expression to describe this
				entb	= ENARR(ENTno);
				entb->TTno		= TDno;				// Tableno
                entb->RecNo		= fno;				// fieldno
                entb->entype	= 1;				// variable/field in table.

				pr->VAR_ENTno = ENTno;
				pr->WIDTH_FLAGS &= 0x7C00u;			//  0111 1100 0000 0000
                if ( v25 )
					pr->WIDTH_FLAGS |= v25;
				else
					pr->WIDTH_FLAGS |= 0x0400;		// normal variable

                if ( fld[1].FLDelemID )				// Is there a field that follows us?
                {
					PRno = getprmem();
					pr->NextPR = PRno;
                    v17 = PRARR(pr->NextPR);
					assert(v17);
                    v17->WIDTH_FLAGS = pr->WIDTH_FLAGS;
                    pr = v17;
				}
                fno++;
                fld++;
			}
		}

        if ( symbol == 930 )		// <CR> we are done.
		{
			dallprpt(&pr);
	        return a1;				// exit success
		}
		else						// not a <CR>. allocate a new PRTAB, and go around again!
		{
			PRno = getprmem();			//*** getprmem() can change pr ***
			pr->NextPR = PRno;
            v17 = PRARR(pr->NextPR);
			pr = v17;
			assert(v17);
		}
        
	}	// end while (symbol)
    dallprpt(&pr);
    return 0;
}

#endif
