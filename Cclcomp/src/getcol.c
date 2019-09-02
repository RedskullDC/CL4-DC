#ifndef GETCOL_C
#define GETCOL_C

#include <stdio.h>
#include <stdarg.h>		// for var args stuff
#include <stdlib.h>         // for strtol()
#include <assert.h>
#include "DBdefs.h"
#include "cl4.h"

XTAB *getcol(XTAB *xt, int type)	// type 1 = Answer, 2 = prompt, 4 = Head/Text commands (don't update statics) 
{
	short	TestSym;
	short	ExpNo;
	short	v7;
	short	v9;
	short	Attr;

	unsigned short run_rel;
	bool	RequireAttr = false;
	
    TestSym = 0;
     
	if ( syml == 1 && symbol == 2230 && sym[0] == 'c' )	// is there a system variable called 'c' ????
		symbol = 170;		// ?????

	if ( symbol == 2230 )				// variable/table field
	{
		ExpNo = loadexp(0, 1);
		if ( ExpNo )
		{
			if ( type == col_ANSWER )
				xt->ACol_exp = ExpNo;
			else
				xt->PCol_exp = ExpNo;
		}
		else
			return 0;					// error exit
	}
	else if ( symbol == 1800 || symbol == 930)  // 1800 (=) or 930 <CR> == use last static column values
	{
        if ( type == col_ANSWER )
		{
			if ( picol_0 <= 0 || picol_0 > _co )	// bounds check
				picol_0 = 1;						// update static input column value
			
			xt->ACol = picol_0;
        }
        else	// col_PROMPT or col_HEAD
        {
			if ( ppcol_1 <= 0 || ppcol_1 > _co )	// bounds check
				v7 = 1;
			else
				v7 = ppcol_1;
			
			if ( type != col_HEAD )
				ppcol_1 = v7;			// update static prompt column value for col_PROMPT only
			xt->PCol = v7;
        }
		if ( symbol != 930 )			// not <CR>
			symbol = getsym();

	}
		//	1820 = ">", 1840 = "<" (runtime relative) or 2200 = Integer literal, 50 = '+', 1580 = '-' 
	else if ( symbol == 1820 || symbol == 1840 || symbol == 2200 || symbol == 50 || symbol == 1580)
	{	
		if ( symbol != 2200 )		// '<', '>', '+', '-'
		{
			TestSym = symbol;		// save for below
			symbol = getsym();
			if ( symbol != 2200 )	// Integer literal
			{
				loaderr(34, sym);	// "invalid column specification"
				return 0;
			}
		}
		v9 = strtol(sym, 0, 10);
		v7 = v9;
		
		if ( v9 < 0 )
			TestSym = 1580;					// "-"

		if ( type == col_ANSWER )
		{
			switch (TestSym)
			{
			case 1840:						// "<"	runtime relative to last address :   <integer 
			case 1820:						// ">"	runtime relative to last address :   >integer
				if ( TestSym == 1840 )
					run_rel = 0x2000u;
				else
					run_rel = 0x1000u;
				
				if ( v9 < 0 )				// shouldn't be possible, as -ve value changes TestSym above to 1580 ["-"]				
				{
					if ( run_rel == 0x2000 )	// switch the flags for negative number
						run_rel = 0x1000u;
					else
						run_rel = 0x2000u;
					v7 = -v9;
				}
				v7 |= run_rel;				// add the run time relative flag bits
				break;
			
			case 50:						// '+' or '-' = compile time relative
			case 1580:
				v7 = picol_0 + v9;			// fall into default case to range check, and update static
			default:
				if ( v7 <= 0 || v7 > _co)	// range sanity check
					v7 = 1;
				picol_0 = v7;				// update static input column counter
				break;
			}
			xt->ACol = v7;
		}
		else		// col_PROMPT or col_HEAD
		{
			if ( TestSym == 50 || TestSym == 1580 ) // '+' or '-'	column specifier is relative to last used value
				v7 = ppcol_1 + v9;
			
			if ( v7 <= 0 || v7 > _co )	// range check
				v7 = 1;					// reset to column #1
				
			if ( type != col_HEAD )
				ppcol_1 = v7;			// update the static ppcol_1 value

			xt->PCol = v7;
		}
		symbol = getsym();
	}
	else if ( syml == 1 )		// look for a single char screen loc specifier 
	{							// Flags only valid for "head" and "text" specifiers
		switch (sym[0])
		{
		case '^':				// left side of screen align
			v7 = 1;				// column 1
			break;
		case 'c':				// centre of screen align
			v7 = 0x1000u;		// other 2 are flag bits
			break;
		case '$':				// right side of screen align
			v7 = 0x2000u;
			break;
		default:
			v7 = 0;
			break;
		}
		if ( v7 )
		{
			if ( type == col_ANSWER )
				xt->ACol = v7;
			else
				xt->PCol = v7;
			symbol = getsym();
		}
	}
	else
	{
		loaderr(34, sym);			// "invalid column specification"
		return 0;
	}

// ========================================
// look for any attribute specifiers
//=========================================
	
	if ( symbol == 180 )		// ','  comma, look for a following attribute specifier
    {
		RequireAttr = true;		// defaults to false above
        symbol = getsym();
	}

	Attr = 0;
	if ( RequireAttr || syml == 1 )	// comma is optional.
    {
		switch ( sym[0] )
		{
			case 'f':
				Attr = 0x0001;		// feint
				break;
			case 's':
				Attr = 0x0002;		// standout
				break;
			case 'u':
				Attr = 0x0004;		// underline
				break;
			case 'b':
				Attr = 0x0008;		// bold
				break;
			case 'B':
				Attr = 0x0010;		// Blink
				break;
			case 'r':
				Attr = 0x0020;		// reverse
				break;
			case 'n':				// normal
			case 'x':
				Attr = 0x003F;				// turn all flags on/off?
				break;
			default:
				if ( RequireAttr )			// If there was no comma, and no match on the single char token,
				{							// then just ignore it. It may be a single char numeric digit
					loaderr(36, sym);		// "invalid screen attribute"
					return 0;
				}
				break;
		}
		if (Attr)	// we had a match above, get next symbol
			symbol = getsym();
	}
    if ( type == col_ANSWER )
		xt->Attr = Attr;
	else
		xt->ScrAttrib = Attr;
	return xt;
}

#endif
